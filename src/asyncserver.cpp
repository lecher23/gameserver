#include "asyncserver.h"

using boost::asio::ip::tcp;
namespace cgserver{
    AsyncServer::AsyncServer(int port)
	:_service(),_acceptor(_service, tcp::endpoint(tcp::v4(), port))
    {
    }
    
    AsyncServer::~AsyncServer(){
    }

    void AsyncServer::run() {
	_service.run();
    }

    void AsyncServer::start() {
	doAccept();
	auto f = std::bind(&AsyncServer::run, this);
	for (int i = 0; i < 8; ++i) {
	    std::thread t(f);
	}
    }

    void AsyncServer::doAccept() {
	AsyncTaskPtr task(new AsyncTask(_service));
	_acceptor.async_accept(
	    task->getSocket(),
	    std::bind(&AsyncServer::handleAccept, this, task, std::placeholders::_1));
    }

    void AsyncServer::stop() {
    }

    void AsyncServer::handleAccept(AsyncTaskPtr task, const asio_error &err) {
	// process this connection
	if (!err) {
	    task->read(task);
	}
	doAccept();
    }    
}
