#include "tcpserver.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include "handlers/packethandler.h"
#include "socket/socketprocessor.h"
#include "util/task.h"

namespace cgserver {
    
TcpServer::TcpServer():_stop(false), _processor(NULL) {
}

TcpServer::~TcpServer() {
    TcpServer::free();
}

bool TcpServer::initServer(int port) {
    cglogic::PacketHandler *handler = new cglogic::PacketHandler();
    _processor = (IProcessor *)(new SocketProcessor((IHandler *)handler));

    if (!_socket.setAddress(NULL, port)){
	std::cout << "Set socket address failed." << std::endl;
	return false;
    }
    
    if (!_socket.listen(256)) {
	std::cout << "Listen failed." << std::endl;
	return false;
    }

    // Init thread pool, default 8 threads and queue size is 512
    _pool.reset(new ThreadPool(8));
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
	Task *task = new Task(clientSocket, _processor);
	if (_pool->pushTask((Runnable *)task) != ThreadPool::ERROR_NONE) {
	    // response failed msg to user.
	    // release mem
	    delete task;
	}
    }
    TcpServer::free();
}

void TcpServer::free() {
    if (_processor) {
	delete _processor;
	_processor = NULL;
    }
}

void TcpServer::stopServer() {
    _stop = true;
}

}
