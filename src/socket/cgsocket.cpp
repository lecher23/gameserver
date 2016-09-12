#include "cgsocket.h"
#include <util/timeutil.h>
#include <boost/bind.hpp>

BEGIN_NAMESPACE(cgserver)

CgSocket::CgSocket(asio_service &service)
:_service(service), _socket(_service),_output(), _msgParser()
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

void CgSocket::afterRead(const asio_error &err, size_t read_len) {
  if (!_connected) {
    return ;
  }
  bool ret = false;
  // some error such as sys busy or try again is special
  if (err || !_msgParser.parse(_input, read_len)){
    // error end process
      CLOG(ERROR) << "error:" << err;
    closeSocket();
    return;
  }
  CLOG(ERROR) << "read " << read_len;
  _socket.async_read_some(
      asio_buffer(_input, MAX_SOCKET_BUFFER_SIZE),
      boost::bind(&CgSocket::afterRead, shared_from_this(),
                  asio_placeholders::error, asio_placeholders::bytes_transferred));
  process();
}

// void CgSocket::sendMsg() {
//   // just append msg to _output;
// }

void CgSocket::start() {
  _connected = true;
  _connStartTime = CTimeUtil::getCurrentTimeInSeconds();
  _timer.reset(new asio_deadline_timer(_service, asio_seconds(MAX_ALIVE_TIME)));
  _timer->async_wait(
      boost::bind(&CgSocket::timeoutCheck, shared_from_this(),
                      asio_placeholders::error));
  _socket.async_read_some(
      asio_buffer(_input, MAX_SOCKET_BUFFER_SIZE),
      boost::bind(&CgSocket::afterRead, shared_from_this(),
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

void CgSocket::process() {
  // return: should write data?
  // make sure data in input is complete.
  // if data is invalid , close socket.
  // at the end of process, send msg to client.
  _lastHeartBeat = CTimeUtil::getCurrentTimeInSeconds();
  std::string packet = _msgParser.nextPacket();
  CLOG(ERROR) << "Get packet:[" << packet << "], empty:" << packet.empty();
  while(!packet.empty()) {
      _output.clear();
      char *o = _output.getFree();
      size_t i = 0;
      for (auto &ch: packet) {
          o[i++] = ch;
      }
      _output.pourData(packet.size());
      packet = _msgParser.nextPacket();
      if (_connected) {
        boost::asio::async_write(
            _socket,asio_buffer(_output.getData(), _output.getDataLen()),
            boost::asio::transfer_at_least(_output.getDataLen()),
            boost::bind(&CgSocket::afterWrite, shared_from_this(),
                        asio_placeholders::error, asio_placeholders::bytes_transferred));
      }
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
