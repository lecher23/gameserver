#include "asynctask.h"

namespace cgserver{
    AsyncTask::AsyncTask(asio_service &service)
	:_service(service), _socket(_service)
    {
	_finish = true;
	_limit = false;
	_input.ensureFree(MAX_BUFFER_SIZE);
	_output.ensureFree(MAX_BUFFER_SIZE);
    }
    
    AsyncTask::~AsyncTask(){
	;
    }

    asio_socket &AsyncTask::getSocket() {
	return _socket;
    }

    bool AsyncTask::process() {
	std::string in(_input.getData(), _input.getDataLen());
	std::cout << "get packet " << in << std::endl;
	char o[] = "HTTP/1.1 200 OK\nConnection: Keep-Alive\nContent-Length: 4\n\nYES\n";
	_output.readBytes(o, sizeof(o));
	return true;
    }

    void AsyncTask::handleWrite(const asio_error &err, size_t write_len) {
	std::cout << "write len:" << write_len << std::endl;
	if (err || write_len == _output.getDataLen()) {
	    _socket.shutdown(asio_socket::shutdown_both);	    
	}
	// if write not finish, continue write
	_output.drainData(write_len);
	boost::asio::async_write(_socket,asio_buffer(_output.getData()),
				 ASIO::transfer_at_least(_output.getDataLen())
				 std::bind(&AsyncTask::handleWrite, shared_from_this(),
					   std::placeholders::_1, std::placeholders::_2));
    }

    void AsyncTask::handleRead(const asio_error &err, size_t read_len) {
	std::cout << "read len" << read_len << std::endl;
	bool ret = false;
	do {
	    // some error such as sys busy or try again is special
	    if (err){
		std::cout << "err ocur" << std::endl;
		// error end process
		return;
	    }
	    _input.pourData(read_len);
	    if (!process()) {
		break;
	    }
	    boost::asio::async_write(_socket,asio_buffer(_output.getData()),
				     ASIO::transfer_at_least(_output.getDataLen())
				     std::bind(&AsyncTask::handleWrite, shared_from_this(),
				     std::placeholders::_1, std::placeholders::_2));
	    ret = true;
	}while (false);
	if (!ret) {
	    _socket.shutdown(asio_socket::shutdown_both);
	}
    }

    void AsyncTask::read() {
	_socket.async_read_some(asio_buffer(_input.getData()),
				std::bind(&AsyncTask::handleRead, shared_from_this(),
			        std::placeholders::_1, std::placeholders::_2));
    }
    
}
