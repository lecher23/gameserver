#include "asyncserver.h"
#include <boost/bind.hpp>

using boost::asio::ip::tcp;
namespace cgserver{
    AsyncServer::AsyncServer(asio_service &service, int port)
	:_service(service),_acceptor(service, tcp::endpoint(tcp::v4(), port), true)
    {
	_stop = false;
    }

    AsyncServer::~AsyncServer(){
    }

    void AsyncServer::start(IHandler *handler) {
	_handler = handler;
	CLOG(INFO) << "start async server.";
	doAccept();
        for (size_t i = 0; i < 8; ++i) {
            std::shared_ptr<std::thread> tmp(
                new std::thread(std::bind(&AsyncServer::run, this)));
            _threads.push_back(tmp);
        }
        for (auto &item: _threads) {
            item->join();
        }
    }

    void AsyncServer::doAccept() {
	if (_stop){
	    return;
	}
	_task.reset(new AsyncConn(_service));
	_acceptor.async_accept(
	    _task->getSocket(),
	    boost::bind(&AsyncServer::handleAccept, this, _task, asio_placeholders::error));
    }

    void AsyncServer::stop() {
	_stop = true;
        _acceptor.close();
    }

    void AsyncServer::handleAccept(AsyncConnPtr task, const asio_error &err) {
	// process this connection
	if (!err) {
	    task->setHandler(_handler);
	    task->startConn();
	}
	// continue to accept
	doAccept();
    }

    void AsyncServer::run(){
        CLOG(INFO) << "Start service in thread.";
        _service.run();
        CLOG(INFO) << "Service end.";
    }
}
