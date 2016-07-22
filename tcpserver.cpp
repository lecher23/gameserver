#include "tcpserver.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include "handlers/packethandler.h"
#include "util/thread.h"

namespace cgserver {
    TcpServer::TcpServer():_stop(false) {
    }

TcpServer::~TcpServer() {}

bool TcpServer::initServer(int port) {
    cglogic::PacketHandler *handler = new cglogic::PacketHandler();
    _processor.initResource((IHandler *)handler);

    if (!_socket.setAddress(NULL, port)){
	std::cout << "Set socket address failed." << std::endl;
	return false;
    }
    
    if (!_socket.listen(256)) {
	std::cout << "Listen failed." << std::endl;
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
	Thread thread;
	thread.start((Runnable *)&_processor, (void *) clientSocket);
    }
}

void TcpServer::stopServer() {
    _stop = true;
}

}
