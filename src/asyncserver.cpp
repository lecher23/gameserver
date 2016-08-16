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
	auto sk_ptr = getSocket();
	_acceptor.asyc_accept(*sk_ptr, );
	auto f = std::bind(&AsyncServer::run, this);
	for (int i = 0; i < 8; ++i) {
	    std::thread t(f);
	}
    }

    void AsyncServer::stop() {
    }

    socket_ptr AsyncServer::getSocket() {
	suto ret = socket_ptr(new tcp::socket(_service));
	sockets.push_back(ret);
	return ret;
    }

    void AsyncServer::handleAccept(socket_ptr sk, const error_code &err) {
    }    
}
