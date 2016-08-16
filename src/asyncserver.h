#ifndef ASYNCSERVER_H
#define ASYNCSERVER_H

#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <memory>
#include <functional>

namespace cgserver{
    class AsyncServer{
    public:
	typedef std::shared_ptr <boost::asio::ip::tcp::socket> socket_ptr;
        AsyncServer();
        ~AsyncServer();

	void start();
	void stop();

	socket_ptr getSocket();

	void handleAccept(socket_ptr sk, const error_code &err);
    private:
	void run();
	boost::asio::io_service _service;
	boost::asio::ip::tcp::acceptor _acceptor;
	std::vector<std::thread> threads;
	std::vector<socket_ptr> sockets;
    };
}
#endif
