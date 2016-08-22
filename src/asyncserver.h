#ifndef ASYNCSERVER_H
#define ASYNCSERVER_H

#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include "asyncconn.h"
#include "handlers/ihandler.h"

namespace cgserver{
    class AsyncServer{
    public:
	typedef std::shared_ptr <boost::asio::ip::tcp::socket> socket_ptr;
        explicit AsyncServer(asio_service &service, int port);
        ~AsyncServer();

	void start(IHandler *handler);
	void stop();

	void handleAccept(AsyncConnPtr task, const asio_error &err);
	void doAccept();	
    private:
	void run();
	boost::asio::io_service &_service;
	boost::asio::ip::tcp::acceptor _acceptor;
	std::vector<std::shared_ptr<std::thread>> threads;
	AsyncConnPtr _task;
	IHandler *_handler;
	
	bool _stop;
	std::shared_ptr<asio_service::work> _work;
    };
}
#endif
