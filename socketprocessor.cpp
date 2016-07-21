#include "socketprocessor.h"
#include "httppacketparser.h"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>

namespace cgserver{
    SocketProcessor::SocketProcessor():_handler(NULL){}

    SocketProcessor::~SocketProcessor(){
	if (_handler != NULL)
	    delete _handler;
	_handler = NULL;
    }
    
    void SocketProcessor::run(Thread *thread, void *arg) {
	std::cout<< "thread run." << std::endl;
	if (_handler == NULL)
	    return;
	int fd = *((int *)arg);
	std::cout << fd << std::endl;
	if (fd < 0)
	    return;
	
	DataBuffer buff;
	buff.ensureFree(MAX_BUFF_SIZE);
	readData(fd, buff);
	HTTPPacketParser parser;
	HTTPPacket packet;
	parser.processData(&buff, &packet);// parse http packet over

	HttpResponsePacket resp;
	std::cout << "---in" << std::endl;
	_handler->process(packet, resp);
	std::cout << "---out" << std::endl;	
	writeData(fd, resp);
	::close(fd);// close socket right now, this will cost some problem.
    }

    void SocketProcessor::readData(int fd, DataBuffer &buf) {
	int bytes_recv = -1;
	int count = 0;
	while(bytes_recv < 0) {
	    bytes_recv = ::recv(fd, buf.getFree(), buf.getFreeLen(), 0);
	    if (bytes_recv >= 0) {
		buf.pourData(bytes_recv);
		break;
	    }
	    if (count++ > 10) break;
	}
	*(buf.getFree()) = '\0';
	std::cout << buf.getData() << std::endl;
    }

    void SocketProcessor::writeData(int fd, HttpResponsePacket &resp) {
	cgserver::DataBuffer output; 
	if (resp.encode(&output)) {
	    std::string data(output.getData(), output.getDataLen());
	    std::cout << data<< std::endl;
	    ::send(fd, output.getData(), output.getDataLen(), 0);
	}
	std::cout << "finish send packet" << std::endl;
    }

    void SocketProcessor::initResource(IHandler *handler){
	if (handler == NULL) return;
	_handler = handler;
	_handler->init();
    }
}
