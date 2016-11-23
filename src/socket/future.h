#ifndef FUTURE_H
#define FUTURE_H

#include "util/common_define.h"
#include "util/timeutil.h"
#include <queue>
#include <boost/bind.hpp>
#include "futuresub.h"

BEGIN_NAMESPACE(cgserver)

#define MAX_ALIVE_TIME 5
#define MAX_SOCKET_BUFFER_SIZE 30

class ConnPub;

DF_SHARED_PTR(ConnPub);

class ConnPub: public std::enable_shared_from_this<ConnPub> {
public:
    typedef std::shared_ptr<ConnSub<ConnPub>> ConnSubPtr;

    ConnPub(asio_service &service): _service(service), _socket(_service){
    }

    ~ConnPub() {closeSocket();}

    asio_socket &getSocket() {return _socket;}

    void addWatcher(ConnSubPtr upload, ConnSubPtr sync) {
        _uploadWatcher = upload;
        _syncWatcher = sync;
    }

    bool connEstablished() {
        return _connEstablished;
    }

    bool establishConn(int32_t id) {
        _connEstablished = true;
        _establishCallback(id, shared_from_this());
    }

    void setEstablishCallback(std::function<void(int32_t, ConnPubPtr)> func) {
        _establishCallback = func;
    }

    void start() {
        _timer.reset(new asio_deadline_timer(_service, asio_seconds(MAX_ALIVE_TIME)));
        _timer->async_wait(
            boost::bind(&ConnPub::timeoutCheck, shared_from_this(),
                        asio_placeholders::error));
        readDataOnce();
    }

    void writeData(const std::string &data) {
        _buffer.push(std::move(data));
        if (!_writing) {
            boost::asio::async_write(
                _socket, asio_buffer(_buffer.front(), _buffer.front().size()),
                boost::asio::transfer_at_least(_buffer.front().size()),
                boost::bind(&ConnPub::writeHandler, shared_from_this(),
                            asio_placeholders::error, asio_placeholders::bytes_transferred));
            _writing = true;
        }
    }

    void readData() {
        ConnPacketPtr packet = std::make_shared<ConnPacket>();
        boost::asio::async_read(
            _socket, asio_buffer(packet->sType, TCP_PACKET_TYPE_SIZE),
            boost::bind(&ConnPub::handleReadType, shared_from_this(), packet,
                        asio_placeholders::error, asio_placeholders::bytes_transferred));
    }

    void readDataOnce() {
        _loopRead = false;
        readData();
    }

    void readDataLoop() {
        _loopRead = true;
        readData();
    }

    void closeSocket() {
        CLOG(INFO) << "close socket.";
        if  (!_connected)  {
            return;
        }
        try{
            _socket.shutdown(asio_socket::shutdown_both);
            _socket.close();
        } catch(std::exception& e) {
            CLOG(WARNING) << "Close socket failed: " << e.what();
        }
        //_connStopTime = CTimeUtil::getCurrentTimeInSeconds();
        _connected = false;
    }

private:
    void handleReadType(ConnPacketPtr packet, const asio_error &err, size_t readLen){
        if (!_connected) {
            return ;
        }
        if (err || readLen != TCP_PACKET_TYPE_SIZE) {
            CLOG(INFO) << "read type failed:" << err;
            closeSocket();
            return;
        }
        CLOG(INFO)<< "read type over:" << std::string(packet->sType, 1);
        boost::asio::async_read(
            _socket, asio_buffer(packet->sData, TCP_PACKET_DATA_LEN_SIZE),
            boost::bind(&ConnPub::handleReadDataLen, shared_from_this(), packet,
                        asio_placeholders::error, asio_placeholders::bytes_transferred));
    }

    void handleReadDataLen(
        ConnPacketPtr packet, const asio_error &err, size_t readLen)
        {
            if (!_connected) {
                return ;
            }
            if (err || readLen != TCP_PACKET_DATA_LEN_SIZE) {
                CLOG(INFO) << "read data len failed:" << err;
                closeSocket();
                return;
            }
            packet->dataLen = ((uint16_t)(packet->sData[0]) << 8) ^ packet->sData[1];
            CLOG(INFO) << "data len:" << packet->dataLen;
            if (packet->dataLen == 0) {
                // process this data.
                _service.post(boost::bind(&ConnPub::processPacket, shared_from_this(), packet));
                if (_loopRead){
                    ConnPacketPtr newPacket = std::make_shared<ConnPacket>();
                    boost::asio::async_read(
                        _socket, asio_buffer(packet->sType, TCP_PACKET_TYPE_SIZE),
                        boost::bind(&ConnPub::handleReadType, shared_from_this(), newPacket,
                                    asio_placeholders::error, asio_placeholders::bytes_transferred));
                }
            } else {
                boost::asio::async_read(
                    _socket, asio_buffer(packet->data, packet->dataLen),
                    boost::bind(&ConnPub::handleProcessPacket, shared_from_this(), packet,
                                asio_placeholders::error, asio_placeholders::bytes_transferred));
            }
        }

    void handleProcessPacket(
        ConnPacketPtr packet, const asio_error &err, size_t readLen) {
        CLOG(INFO)<< "read data over:" << packet->data << ", len:" << readLen;
        if (!_connected) {
            return ;
        }
        if (err || readLen != packet->dataLen) {
            CLOG(INFO) << "read data failed:" << err;
            closeSocket();
            return;
        }
        // process this data.
        _service.post(boost::bind(&ConnPub::processPacket, shared_from_this(), packet));
        if (_loopRead) {
            // read next packet.
            ConnPacketPtr newPacket = std::make_shared<ConnPacket>();
            boost::asio::async_read(
                _socket, asio_buffer(packet->sType, TCP_PACKET_TYPE_SIZE),
                boost::bind(&ConnPub::handleReadType, shared_from_this(), newPacket,
                            asio_placeholders::error, asio_placeholders::bytes_transferred));
        }
    }

    void writeHandler(const asio_error &err, size_t write_len) {
        if (!_connected) {
            return ;
        }
        if (err) {
            CLOG(INFO) << "write failed:" << err;
            closeSocket();
            return;
        }
        CLOG(INFO) << "write :" << write_len;
        // pop head;
        _buffer.pop();
        if (_buffer.empty()) {
            _writing = false;
        } else {
            boost::asio::async_write(
                _socket, asio_buffer(_buffer.front(), _buffer.front().size()),
                boost::asio::transfer_at_least(_buffer.front().size()),
                boost::bind(&ConnPub::writeHandler, shared_from_this(),
                            asio_placeholders::error, asio_placeholders::bytes_transferred));
            _writing = true;
        }
    }

    void processPacket(ConnPacketPtr packet) {
        _lastHeartBeat = CTimeUtil::getCurrentTimeInSeconds();
        CLOG(INFO) << "Get packet:[" << packet->data << "]";
        if (_connected) {
            if (packet->sType[0] == 'u') {
                _uploadWatcher->update(shared_from_this(), packet);
            }else if (packet->sType[0] == 's') {
                _syncWatcher->update(shared_from_this(), packet);
            }else {
                closeSocket();
            }
        }
    }

    void timeoutCheck(const asio_error &err) {
        CLOG(INFO) << "timeout check.";
        int64_t now = CTimeUtil::getCurrentTimeInSeconds();
        if (now - _lastHeartBeat > MAX_ALIVE_TIME) {
            closeSocket();
            return;
        }
        _timer->expires_at(_timer->expires_at() + asio_seconds(MAX_ALIVE_TIME));
        _timer->async_wait(
            boost::bind(&ConnPub::timeoutCheck, shared_from_this(),
                        asio_placeholders::error));
    }

    asio_service &_service;
    asio_socket _socket;
    asio_deadline_timer_ptr _timer;

    ConnSubPtr _uploadWatcher;
    ConnSubPtr _syncWatcher;

    bool _loopRead{false};
    bool _writing{false};
    bool _connected{true};
    bool _connEstablished{false};

    std::queue<std::string> _buffer;
    std::mutex _lock;
    int64_t _lastHeartBeat{0};
    std::function<void(int32_t, ConnPubPtr)> _establishCallback;
};

END_NAMESPACE
#endif
