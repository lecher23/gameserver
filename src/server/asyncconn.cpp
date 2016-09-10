#include "asyncconn.h"
#include <boost/bind.hpp>
#include <http/httppacketparser.h>
#include <util/requestvalidator.h>

BEGIN_NAMESPACE(cgserver)

AsyncConn::AsyncConn(asio_service &service)
    :_service(service), _socket(_service)
{
    _finish = true;
    _limit = false;
    _connected = true;
    _input.ensureFree(MAX_BUFFER_SIZE);
    _output.ensureFree(MAX_BUFFER_SIZE);
}

AsyncConn::~AsyncConn(){
    ;
}

void AsyncConn::setHandler(IHandler *handler) {
    _handler = handler;
}

asio_socket &AsyncConn::getSocket() {
    return _socket;
}

bool AsyncConn::process() {
    bool ret = false;
    do {
        HTTPPacketParser parser;
        HTTPPacket packet;
        /* Parser http packet from buffer.*/
        if (!parser.processData(&_input, &packet)) {
            CLOG(WARNING) << "Parse http packet failed.";
            break;
        }
        CLOG(INFO) << "Reveive:" << packet.getURI();
        /* Check if it is valid http request.*/
        if (!validatePacket(packet)) {
            CLOG(WARNING) << "validate http packet failed.";
            break;
        }
        HttpResponsePacket resp;
        if (!_handler->process(packet, resp)) {
            CLOG(WARNING) << "process http packet failed.";
            break;
        }
        resp.encode(&_output);
        ret = true;
    } while(0);
    return ret;
}

void AsyncConn::afterWrite(const asio_error &err, size_t write_len) {
    if(!_connected) {
      return;
    }
    if (err || write_len == _output.getDataLen()) {
        closeSocket();
        return;
    }
    // if write not finish, continue write
    _output.drainData(write_len);
    boost::asio::async_write(_socket,asio_buffer(_output.getData(), _output.getDataLen()),
                             boost::asio::transfer_at_least(_output.getDataLen()),
                             boost::bind(&AsyncConn::afterWrite, shared_from_this(),
                                       asio_placeholders::error, asio_placeholders::bytes_transferred));
}

void AsyncConn::afterRead(const asio_error &err, size_t read_len) {
    if (!_connected) {
        return ;
    }
    bool ret = false;
    do {
        // some error such as sys busy or try again is special
        if (err){
            // error end process
            break;
        }
        _input.pourData(read_len);
        if (!process()) {
            break;
        }
        boost::asio::async_write(_socket,asio_buffer(_output.getData(), _output.getDataLen()),
                                 boost::asio::transfer_at_least(_output.getDataLen()),
                                 boost::bind(&AsyncConn::afterWrite, shared_from_this(),
                                             asio_placeholders::error, asio_placeholders::bytes_transferred));
        ret = true;
    }while (false);
    if (!ret) {
        closeSocket();
    }
}

void AsyncConn::startConn() {
    _timer.reset(new asio_deadline_timer(_service, asio_seconds(CONN_EXPIRE_TIME)));
    _timer->async_wait(
        boost::bind(&AsyncConn::disconnect, shared_from_this(), asio_placeholders::error));
    _socket.async_read_some(asio_buffer(_input.getData(), _output.getFreeLen()),
                            boost::bind(&AsyncConn::afterRead, shared_from_this(),
                                        asio_placeholders::error, asio_placeholders::bytes_transferred));
}

bool AsyncConn::validatePacket(HTTPPacket &packet) const {
    if (true) {
        return true;
    }
    return RequestValidator::validate(packet, "secret");
}

void AsyncConn::closeSocket() {
    if  (!_connected)  {
        return;
    }
    try{
        _socket.shutdown(asio_socket::shutdown_both);
        _socket.close();
    } catch(std::exception& e) {
        CLOG(WARNING) << "Close socket failed: " << e.what();
    }
    _connected = false;
}

void AsyncConn::disconnect(const asio_error &err) {
  if (err) {
    CLOG(INFO) << "disconnect meet error:" << err;
    return;
  }
  closeSocket();
}

END_NAMESPACE
