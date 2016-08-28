#ifndef ASYNCCONN_H
#define ASYNCCONN_H

#define BOOST_SYSTEM_NO_DEPRECATED

#include <array>
#include <memory>
#include <iostream>
#include "http/httpresponsepacket.h"
#include "handlers/ihandler.h"
#include "util/databuffer.h"
#include <boost/asio.hpp>

namespace cgserver{
#define asio_buffer(T, S) boost::asio::buffer(T, S)
#define ASIO boost::asio
#define MAX_BUFFER_SIZE 2048
    typedef boost::asio::ip::tcp::socket asio_socket;
    typedef boost::asio::ip::tcp::acceptor asio_acceptor;
    typedef boost::asio::ip::tcp::endpoint asio_endpoint;
    typedef boost::asio::io_service asio_service;
    typedef boost::system::error_code asio_error;

    class AsyncConn: public std::enable_shared_from_this< AsyncConn >
    {
    public:
	explicit AsyncConn(asio_service &service);
        ~AsyncConn();
	void setHandler(IHandler *handler);

	void startConn();
	asio_socket &getSocket();
	HttpResponsePacket &getResponse();

    private:
	bool process();
	void afterWrite(const asio_error &err, size_t write_len);
	void afterRead(const asio_error &err, size_t read_len);
	bool validatePacket(HTTPPacket &packet) const;
        void closeSocket();

        HttpResponsePacket _resp;
	asio_service &_service;
	asio_socket _socket;
	DataBuffer _input;
	DataBuffer _output;
	bool _finish;
	bool _limit;

	IHandler *_handler;
    };
    typedef std::shared_ptr<AsyncConn> AsyncConnPtr;
}
#endif
