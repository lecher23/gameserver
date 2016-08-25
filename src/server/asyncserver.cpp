#include "asyncserver.h"

using boost::asio::ip::tcp;
namespace cgserver{
    AsyncServer::AsyncServer(asio_service &service, int port)
	:_service(service),_acceptor(service, tcp::endpoint(tcp::v4(), port), true)
    {
	_stop = false;
    }

    AsyncServer::~AsyncServer(){
	stop();
	//_work.reset();
    }

    void AsyncServer::start(IHandler *handler) {
	_handler = handler;
	CLOG(INFO) << "start async server.";
	doAccept();
    }

    void AsyncServer::doAccept() {
	if (_stop){
	    return;
	}
	_task.reset(new AsyncConn(_service));
	_acceptor.async_accept(
	    _task->getSocket(),
	    std::bind(&AsyncServer::handleAccept, this, _task, std::placeholders::_1));
    }

    void AsyncServer::stop() {
	//this->_work.reset();
	_stop = true;
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
}
