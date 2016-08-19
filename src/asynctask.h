#ifndef ASYNCTASK_H
#define ASYNCTASK_H

// to solve multiple definition of throws
#define BOOST_SYSTEM_NO_DEPRECATED

#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <iostream>
#include "http/httpresponsepacket.h"
#include "util/databuffer.h"

namespace cgserver{
#define asio_buffer(T) boost::asio::buffer(T)
#define ASIO boost::asio
#define MAX_BUFFER_SIZE 2048    
    typedef boost::asio::ip::tcp::socket asio_socket;
    typedef boost::asio::ip::tcp::acceptor asio_acceptor;
    typedef boost::asio::ip::tcp::endpoint asio_endpoint;
    typedef boost::asio::io_service asio_service;
    typedef boost::system::error_code asio_error;

    class AsyncTask: public std::enable_shared_from_this< AsyncTask >
    {
    public:
	explicit AsyncTask(asio_service &service);
        ~AsyncTask();

	void handleWrite(const asio_error &err, size_t write_len);
	void handleRead(const asio_error &err, size_t read_len);
	void read();
	bool process();
	asio_socket &getSocket();
	HttpResponsePacket &getResponse();
    private:
	HttpResponsePacket _resp;
	asio_service &_service;
	asio_socket _socket;
	DataBuffer _input;
	DataBuffer _output;
	bool _finish;
	bool _limit;
    };
    typedef std::shared_ptr<AsyncTask> AsyncTaskPtr;
    
}
#endif
