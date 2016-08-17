#include "asynctask.h"

namespace cgserver{
    AsyncTask::AsyncTask(asio_service &service)
	:_service(service), _socket(_service)
    {
	_finish = true;
	_limit = false;
    }
    
    AsyncTask::~AsyncTask(){
	;
    }

    asio_socket &AsyncTask::getSocket() {
	return _socket;
    }

    bool AsyncTask::process() {
	return true;
    }

    void AsyncTask::handleWrite(const asio_error &err, size_t write_len) {
	_socket.shutdown(asio_socket::shutdown_both);
    }

    void AsyncTask::handleRead(const asio_error &err, size_t read_len) {
	std::cout << "read" << std::endl;
	bool ret = false;
	do {
	    // if (!_finish) {
	    // 	if (_limit){
	    // 	    read(task);
	    // 	    ret = true;
	    // 	}
	    // 	break;
	    // }
	    if (!process()) {
		break;
	    }
	    _socket.async_write_some(asio_buffer(_buffer),
				     std::bind(&AsyncTask::handleRead, shared_from_this(),
				     std::placeholders::_1, std::placeholders::_2));
	    ret = true;
	}while (false);
	if (!ret) {
	    _socket.shutdown(asio_socket::shutdown_both);
	}
    }

    void AsyncTask::read() {
	_socket.async_read_some(asio_buffer(_buffer),
				std::bind(&AsyncTask::handleRead, shared_from_this(),
			        std::placeholders::_1, std::placeholders::_2));
    }
    
}
