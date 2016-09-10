#include "cgsocket.h"
#include <util/timeutil.h>
#include <boost/bind.hpp>

BEGIN_NAMESPACE(cgserver)

CgSocket::CgSocket(asio_service &service)
  :_service(service), _socket(_service)
{
  _lastHeartBeat = CTimeUtil::getCurrentTimeInSeconds();
  _input.ensureFree(MAX_BUFFER_SIZE);
  _output.ensureFree(MAX_BUFFER_SIZE);
}

CgSocket::~CgSocket(){
}

asio_socket &CgSocket::getSocket() {
    return _socket;
}

void CgSocket::afterWrite(const asio_error &err, size_t write_len) {
  if (err) {
    CLOG(INFO) << "write failed:" << err;
    closeSocket();
    return;
  }
  if (write_len == _output.getDataLen()) {
    return;
  }
  // if write not finish, continue write
  _output.drainData(write_len);
  boost::asio::async_write(_socket,asio_buffer(_output.getData(), _output.getDataLen()),
                           boost::asio::transfer_at_least(_output.getDataLen()),
                           boost::bind(&CgSocket::afterWrite, shared_from_this(),
                                       asio_placeholders::error, asio_placeholders::bytes_transferred));
}

void CgSocket::afterRead(const asio_error &err, size_t read_len) {
  bool ret = false;
  // some error such as sys busy or try again is special
  if (err){
    // error end process
    closeSocket();
    return;
  }
  _input.pourData(read_len);
  process();
}

void CgSocket::start() {
  _socket.async_read_some(asio_buffer(_input.getData(), _output.getFreeLen()),
                          boost::bind(&CgSocket::afterRead, shared_from_this(),
                                      asio_placeholders::error, asio_placeholders::bytes_transferred));
}

void CgSocket::closeSocket() {
    try{
        _socket.shutdown(asio_socket::shutdown_both);
        _socket.close();
    } catch(std::exception& e) {
        CLOG(WARNING) << "Close socket failed: " << e.what();
    }
}

void CgSocket::process() {
  // return: should write data?
  // make sure data in input is complete.
  // if data is invalid , close socket.
  // at the end of process, send msg to client.
  boost::asio::async_write(_socket,asio_buffer(_output.getData(), _output.getDataLen()),
                           boost::asio::transfer_at_least(_output.getDataLen()),
                           boost::bind(&CgSocket::afterWrite, shared_from_this(),
                                       asio_placeholders::error, asio_placeholders::bytes_transferred));
}
END_NAMESPACE
