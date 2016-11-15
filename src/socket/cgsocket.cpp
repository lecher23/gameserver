#include "cgsocket.h"
#include <util/timeutil.h>
#include <boost/bind.hpp>

BEGIN_NAMESPACE(cgserver)

CgSocket::CgSocket(asio_service &service)
:_service(service), _socket(_service),_output()
{
  _connStartTime = 0;
  _lastHeartBeat = 0;
  _connected = false;
  _output.ensureFree(MAX_BUFFER_SIZE);
}

CgSocket::~CgSocket(){
}

asio_socket &CgSocket::getSocket() {
  return _socket;
}

void CgSocket::afterWrite(const asio_error &err, size_t write_len) {
  if (!_connected) {
    return ;
  }
  if (err) {
    CLOG(INFO) << "write failed:" << err;
    closeSocket();
    return;
  }
  CLOG(INFO) << "write :" << write_len;
  // if (write_len == _output.getDataLen()) {
  //   // clear output buffer
  //   _output.clear();
  // } else {
  //   // if write not finish, continue write
  //   _output.drainData(write_len);
  //   boost::asio::async_write(_socket,asio_buffer(_output.getData(), _output.getDataLen()),
  //                            boost::asio::transfer_at_least(_output.getDataLen()),
  //                            boost::bind(&CgSocket::afterWrite, shared_from_this(),
  //                                        asio_placeholders::error, asio_placeholders::bytes_transferred));
  // }
}

void CgSocket::start() {
    CLOG(INFO)<< "conn start.";
    _connected = true;
    _connStartTime = CTimeUtil::getCurrentTimeInSeconds();
    _timer.reset(new asio_deadline_timer(_service, asio_seconds(MAX_ALIVE_TIME)));
    _timer->async_wait(
        boost::bind(&CgSocket::timeoutCheck, shared_from_this(),
                    asio_placeholders::error));
    TCPPacketPtr packet = std::make_shared<TCPPacket>();
    boost::asio::async_read(
        _socket, asio_buffer(packet->sType, TCP_PACKET_TYPE_SIZE),
        boost::bind(&CgSocket::handleReadType, shared_from_this(), packet,
                    asio_placeholders::error, asio_placeholders::bytes_transferred));
    // there also has an task for write some data.
    // it will loop to check weather data is ready to write.
}

void CgSocket::handleReadType(TCPPacketPtr packet, const asio_error &err, size_t readLen){
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
        boost::bind(&CgSocket::handleReadDataLen, shared_from_this(), packet,
                    asio_placeholders::error, asio_placeholders::bytes_transferred));
}

void CgSocket::handleReadDataLen(
    TCPPacketPtr packet, const asio_error &err, size_t readLen)
{
    CLOG(INFO)<< "read data len over:" << std::string(packet->sData, 2);
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
        _service.post(boost::bind(&CgSocket::processPacket, shared_from_this(), packet));
        // read next packet.
        TCPPacketPtr newPacket = std::make_shared<TCPPacket>();
        boost::asio::async_read(
            _socket, asio_buffer(packet->sType, TCP_PACKET_TYPE_SIZE),
            boost::bind(&CgSocket::handleReadType, shared_from_this(), newPacket,
                        asio_placeholders::error, asio_placeholders::bytes_transferred));
        return;
    }
    //packet->data.resize(packet->dataLen);
    boost::asio::async_read(
        _socket, asio_buffer(packet->data, packet->dataLen),
        boost::bind(&CgSocket::handleProcessPacket, shared_from_this(), packet,
                    asio_placeholders::error, asio_placeholders::bytes_transferred));
}

void CgSocket::handleProcessPacket(
    TCPPacketPtr packet, const asio_error &err, size_t readLen)
{
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
    _service.post(boost::bind(&CgSocket::processPacket, shared_from_this(), packet));
    // read next packet.
    TCPPacketPtr newPacket = std::make_shared<TCPPacket>();
    boost::asio::async_read(
        _socket, asio_buffer(packet->sType, TCP_PACKET_TYPE_SIZE),
        boost::bind(&CgSocket::handleReadType, shared_from_this(), newPacket,
                    asio_placeholders::error, asio_placeholders::bytes_transferred));
}

void CgSocket::closeSocket() {
  if  (!_connected)  {
    return;
  }
  try{
    _socket.shutdown(asio_socket::shutdown_both);
    _socket.close();
  } catch(std::exception& e) {
    CLOG(WARNING) << "Close socket failed: " << e.what();
  }
  _connStopTime = CTimeUtil::getCurrentTimeInSeconds();
  _connected = false;
}

void CgSocket::processPacket(TCPPacketPtr packet) {
  // if data is invalid , close socket.
  // at the end of process, send msg to client.
  _lastHeartBeat = CTimeUtil::getCurrentTimeInSeconds();
  CLOG(ERROR) << "Get packet:[" << packet->data << "]";
  if (_connected) {
      // add data to queue that to be sent.
      // boost::asio::async_write(
      //     _socket,asio_buffer(_output.getData(), _output.getDataLen()),
      //     boost::asio::transfer_at_least(_output.getDataLen()),
      //     boost::bind(&CgSocket::afterWrite, shared_from_this(),
      //                 asio_placeholders::error, asio_placeholders::bytes_transferred));
      ;
  }
}

void CgSocket::timeoutCheck(const asio_error &err) {
  int64_t now = CTimeUtil::getCurrentTimeInSeconds();
  if (now - _lastHeartBeat > MAX_ALIVE_TIME) {
    closeSocket();
    return;
  }
  _timer->expires_at(_timer->expires_at() + asio_seconds(MAX_ALIVE_TIME));
  _timer->async_wait(
      boost::bind(&CgSocket::timeoutCheck, shared_from_this(),
                  asio_placeholders::error));
}

END_NAMESPACE
