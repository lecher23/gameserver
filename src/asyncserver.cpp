#include "asyncserver.h"

using boost::asio::ip::tcp;
namespace cgserver{
    AsyncServer::AsyncServer(asio_service &service, int port)
	:_service(service),_acceptor(service, tcp::endpoint(tcp::v4(), port))
    {
	_stop = false;
    }
    
    AsyncServer::~AsyncServer(){
	std::cout <<"stop thread" << std::endl;
	stop();
	//_work.reset();
    }

    void AsyncServer::run() {
	//doAccept();
	_work.reset(new asio_service::work(_service));
	_service.run();
    }

    void AsyncServer::start(IHandler *handler) {
	_handler = handler;
	std::cout << "start server." << std::endl;	
	doAccept();
	auto f = std::bind(&AsyncServer::run, this);
	for (int i = 0; i < 8; ++i) {
	    std::shared_ptr<std::thread>
	    	tPtr(new std::thread(std::bind(&asio_service::run, &_service)));
	    // std::shared_ptr<std::thread>
	    // 	tPtr(new std::thread(std::bind(&AsyncServer::run, this)));	    
	    threads.push_back(tPtr);
	}
	std::cout << "service run" << std::endl;
	//_service.run();
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
	_service.stop();
	_stop = true;
	for (auto &it: threads) {
	    it->join();
	}
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
