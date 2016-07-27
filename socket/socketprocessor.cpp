#include "socketprocessor.h"
#include "../http/httppacketparser.h"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include "../handlers/ihandler.h"
#include "../mysql/mysqlclient.h"

namespace cgserver{
    SocketProcessor::SocketProcessor():_handler(NULL){
    }

    SocketProcessor::~SocketProcessor(){
	if (_handler != NULL) {
	    delete _handler;
	    _handler = NULL;
	}
    }

    void SocketProcessor::init(void *resource) {
	if (resource == NULL){
	    return;
	}
	if (_handler != NULL) {
	    delete _handler;
	}
	_handler = (IHandler *)resource;
	_handler->init();
    }

    void SocketProcessor::process(SocketPtr sk) const{
	if (sk == NULL || _handler == NULL) {
	    std::cout << "NULL socket or handler." << std::endl;
	    return;
	}
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
	    std::cout << "**Send response success**" << std::endl;
	} while(0);
    }

    bool SocketProcessor::readData(SocketPtr &sk, DataBuffer &buf) const{
	int bytes_recv = -1;
	int count = 0;
	bytes_recv = sk->read(buf.getFree(), buf.getFreeLen());
	if (bytes_recv < 0) {
	    return false;
	}
	buf.pourData(bytes_recv);
	*(buf.getFree()) = '\0';
	MysqlClient::getInstance().log(std::string(buf.getData()));
	return true;
    }

    bool SocketProcessor::writeData(SocketPtr &sk, HttpResponsePacket &resp) const{
	cgserver::DataBuffer output; 
	if (!resp.encode(&output)) {
	    return false;
	}
	return sk->write(output.getData(), output.getDataLen()) >= 0;
    }

}
