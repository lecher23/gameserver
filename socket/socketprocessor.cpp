#include "socketprocessor.h"
#include "../http/httppacketparser.h"
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
	if (_handler == NULL || arg == NULL)
	    return;
	Socket *sk = (Socket *) arg;
	DataBuffer buff;
	buff.ensureFree(MAX_BUFF_SIZE);
	do {
	    if (!readData(sk, buff)) {
		break;
	    }
	    HTTPPacketParser parser;
	    HTTPPacket packet;
	    if (!parser.processData(&buff, &packet)) {
		break;
	    }
	    HttpResponsePacket resp;
	    _handler->process(packet, resp);
	    if (!writeData(sk, resp)) {
		std::cout << "Send response failed." << std::endl;
		break;
	    }
	    std::cout << "Send response success." << std::endl;
	} while(0);
	delete sk;
    }

    bool SocketProcessor::readData(Socket *sk, DataBuffer &buf) {
	int bytes_recv = -1;
	int count = 0;
	bytes_recv = sk->read(buf.getFree(), buf.getFreeLen());
	if (bytes_recv < 0) {
	    return false;
	}
	buf.pourData(bytes_recv);
	*(buf.getFree()) = '\0';
	std::cout <<"recv data:"<<buf.getData() << std::endl;
	return true;
    }

    bool SocketProcessor::writeData(Socket *sk, HttpResponsePacket &resp) {
	cgserver::DataBuffer output; 
	if (!resp.encode(&output)) {
	    return false;
	}
	return sk->write(output.getData(), output.getDataLen()) >= 0;
    }

    void SocketProcessor::initResource(IHandler *handler){
	if (handler == NULL) return;
	_handler = handler;
	_handler->init();
    }
}
