#include "tcpserver.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include "handlers/packethandler.h"
#include "handlers/handlerfactory.h"
#include "socket/socketprocessor.h"
#include "util/task.h"
#include "util/processorfactory.h"
#include "util/luatoolfactory.h"
#include "util/config.h"

namespace cgserver {
    
TcpServer::TcpServer():_stop(false){
}

TcpServer::~TcpServer() {
}

bool TcpServer::initServer(int port) {
    IHandler *handler = HandlerFactory::getHandler();
    _processor = ProcessorFactory::getProcessor();
    _processor->init((void *)handler);

    std::string file = Config::getInstance().getConfigValue("lua", "path");
    int poolSize;
    if (!Config::getInstance().getIntValue("server", "threadpool_size", poolSize) ||
	poolSize < 0)
    {
	std::cout << "Invalid threadpoll size:" << poolSize << std::endl;
	return false;
    }
    if (!file.empty() && !LuaToolFactory::getInstance().init(poolSize + 2, file)) {
	std::cout << "Init lua factory failed." << std::endl;
	return false;
    }

    if (!_socket.setAddress(NULL, port)){
	std::cout << "Set socket address failed." << std::endl;
	return false;
    }
    
    if (!_socket.listen(256)) {
	std::cout << "Listen failed." << std::endl;
	return false;
    }

    // Init thread pool, default 8 threads and queue size is 512
    _pool.reset(new ThreadPool(poolSize));
    if (!_pool->start()) {
	std::cout << "Create thread poll failed." << std::endl;
	return false;
    }
    return true;
}

void TcpServer::startServer(int port) {
    if (!initServer(port)){
	std::cout << "Init Server failed.!" << std::endl;
	return;
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
	    // response failed msg to user.
	    // release mem
	    delete task;
	}
    }
}

void TcpServer::stopServer() {
    _stop = true;
}

}
