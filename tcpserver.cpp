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

bool TcpServer::initServer(AddrSpec *addr) {
    cglogic::PacketHandler *handler = new cglogic::PacketHandler();
    _processor.initResource((IHandler *)handler);
    
    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd < 0) {
	std::cout << "create socket failed" << std::endl;
	return false;
    }
    int bind_res = bind(_fd, addr->getAddr(), addr->getAddrSize());
    if (bind_res < 0) {
	std::cout << "bind socket failed" << std::endl;	
	return false;
    }
    int listen_res = listen(_fd, 256);
    if (listen_res < 0) {
	std::cout << "listen failed" << std::endl;		
	return false;
    }
    return true;
}

void TcpServer::startServer(AddrSpec *addr) {
    if (!initServer(addr)) {return;}
    int conn_fd;
    struct sockaddr addr_in;
    socklen_t addr_in_len;
    while(!_stop) {
	if ((conn_fd = accept(_fd, &addr_in, &addr_in_len)) < 0) {
	    //accept failed.
	    continue;
	}
	processConnection(conn_fd);
    }
}

void TcpServer::stopServer() {
    _stop = true;
}

void TcpServer::processConnection(int fd){
    Thread thread;
    thread.start((Runnable *)&_processor, (void *) &fd);
}
}
