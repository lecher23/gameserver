#include "tcpserver.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "handlers/handlerfactory.h"
#include "socket/socketprocessor.h"
#include <netinet/in.h>
#include <iostream>
#include "util/task.h"
#include "util/processorfactory.h"
#include <asyncserver.h>
#include <util/config.h>


namespace cgserver {
    
TcpServer::TcpServer():_stop(false), _handler(NULL){
}

TcpServer::~TcpServer() {
}

bool TcpServer::initServer(int port) {
    _handler = HandlerFactory::getHandler();
    _processor = ProcessorFactory::getProcessor();
    _processor->init((void *)_handler);

    return true;
}

    bool TcpServer::startRowServer(int port) {
        if (!_socket.setAddress(NULL, port)){
            CLOG(ERROR) << "Set socket address failed.";
            return false;
        }
        if (!_socket.listen(256)) {
            CLOG(ERROR) << "Listen failed.";
            return false;
        }

        _pool.reset(new ThreadPool(8));
        if (!_pool->start()) {
            CLOG(ERROR) << "Create thread poll failed.";
            return false;
        }

        Socket *clientSocket;
        while(!_stop) {
            if ((clientSocket = _socket.accept()) == NULL) {
                //accept failed.
                continue;
            }
            SocketPtr tmp(clientSocket);
            Task *task = new Task(tmp, _processor);
            if (_pool->pushTask((Runnable *)task) != ThreadPool::ERROR_NONE) {
                // release mem
                delete task;
            }
        }
    }

    bool TcpServer::startAsyncServer(int port) {
        asio_service service;
        AsyncServer server(service, port);
        server.start(_handler);
        service.run();
    }

void TcpServer::startServer(int port) {
    if (!initServer(port)){
	std::cout << "Init Server failed.!" << std::endl;
	return;
    }
    int use_asio;
    if (!Config::getInstance().getIntValue("server", "use_asio", use_asio)) {
        use_asio = 0;
    }
    if (use_asio == 0) {
        startRowServer(port);
    }else {
        startAsyncServer(port);
    }
}

void TcpServer::stopServer() {
    _stop = true;
}

}
