#include "tcpserver.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>

namespace cgserver {
    TcpServer::TcpServer():_stop(false) {
	_buf.ensureFree(MAX_BUFF_SIZE);
    }

TcpServer::~TcpServer() {}

bool TcpServer::initServer(AddrSpec *addr) {
    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd < 0) {
	return false;
    }
    int bind_res = bind(_fd, addr->getAddr(), addr->getAddrSize());
    if (bind_res < 0) {
	return false;
    }
    int listen_res = listen(_fd, 256);
    if (listen_res < 0) {
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
    int bytes_recv = -1;
    int count = 0;
    while(bytes_recv < 0) {
	bytes_recv = recv(fd, _buf.getFree(), _buf.getFreeLen(), 0);
	if (bytes_recv >= 0) {
	    _buf.pourData(bytes_recv);
	    break;
	}
	if (count++ > 10) break;
    }
    *(_buf.getFree()) = '\0';
    std::cout << _buf.getData() << std::endl;
    _handler.handle(_buf, fd);
}
}
