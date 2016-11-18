#ifndef FUTURE_H
#define FUTURE_H

#include "util/common_define.h"
#include "util/timeutil.h"
#include <queue>
#include <boost/bind.hpp>
#include "futuresub.h"

BEGIN_NAMESPACE(cgserver)

#define MAX_ALIVE_TIME 30
#define MAX_SOCKET_BUFFER_SIZE 30

class ConnPub: public std::enable_shared_from_this<ConnPub> {
public:
    typedef std::shared_ptr<ConnSub<ConnPub>> ConnSubPtr;

    ConnPub(asio_service &service): _service(service), _socket(_service){
    }

    ~ConnPub() {closeSocket();}

    asio_socket &getSocket() {return _socket;}

    size_t addWatcher(ConnSubPtr suber) {
        _subscribers.push_back(suber);
        return _subscribers.size() - 1;
    }

    void removeWatcher(size_t index) {
        _subscribers.erase(_subscribers.begin() + index);
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
        CLOG(INFO)<< "read type over:" << std::string(packet->sType, 1);
        if (!_connected) {
            return ;
        }
        if (err || readLen != TCP_PACKET_TYPE_SIZE) {
            CLOG(INFO) << "read type failed:" << err;
            closeSocket();
            return;
        }
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
            for (auto &item: _subscribers) {
                item->update(shared_from_this(), packet);
            }
        }
    }

    asio_service &_service;
    asio_socket _socket;
    std::vector<ConnSubPtr> _subscribers;
    bool _loopRead{false};
    bool _writing{false};
    bool _connected{true};
    std::queue<std::string> _buffer;
    std::mutex _lock;
    int64_t _lastHeartBeat{0};
};
DF_SHARED_PTR(ConnPub);

END_NAMESPACE
#endif
