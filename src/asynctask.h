#ifndef ASYNCTASK_H
#define ASYNCTASK_H

// to solve multiple definition of throws
#define BOOST_SYSTEM_NO_DEPRECATED

#include <boost/asio.hpp>
#include <array>
#include <memory>
#include "http/httpresponsepacket.h"

namespace cgserver{
#define asio_buffer(T) boost::asio::buffer(T)
    typedef boost::asio::ip::tcp::socket asio_socket;
    typedef boost::asio::ip::tcp::acceptor asio_acceptor;
    typedef boost::asio::ip::tcp::endpoint asio_endpoint;
    typedef boost::asio::io_service asio_service;
    typedef boost::system::error_code asio_error;

    class AsyncTask;
    typedef std::shared_ptr<AsyncTask> AsyncTaskPtr;    

    class AsyncTask{
    public:
	explicit AsyncTask(asio_service &service);
        ~AsyncTask();

	void handleWrite(AsyncTaskPtr task, const asio_error &err, size_t write_len);
	void handleRead(AsyncTaskPtr task, const asio_error &err, size_t read_len);
	void read(AsyncTaskPtr task);
	bool process();
	asio_socket &getSocket();
	HttpResponsePacket &getResponse();
    private:
	HttpResponsePacket _resp;
	asio_service &_service;
	asio_socket _socket;
	std::array<char, 1024> _buffer;
	bool _finish;
	bool _limit;
    };

}
#endif
