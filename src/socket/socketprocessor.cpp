#include "socketprocessor.h"
#include "../http/httppacketparser.h"
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include "../handlers/ihandler.h"
#include "../util/config.h"
#include "../util/requestvalidator.h"

namespace cgserver{
    SocketProcessor::SocketProcessor():_handler(NULL){
    }

    SocketProcessor::~SocketProcessor(){
	if (_handler != NULL) {
	    delete _handler;
	    _handler = NULL;
	}
    }

    bool SocketProcessor::init(void *resource) {
	if (resource == NULL){
	    return false;
	}
	if (_handler != NULL) {
	    delete _handler;
	}
        _needCheck = Config::getInstance().needCheckNet();
        if (_needCheck) {
            _secret = Config::getInstance().getNetSecret();
        }
	_handler = (IHandler *)resource;
	return _handler->init();
    }

    void SocketProcessor::process(SocketPtr sk) const{
	if (sk == NULL || _handler == NULL) {
	    CLOG(ERROR) << "NULL socket or handler.";
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
	    /* Parser http packet from buffer.*/
	    if (!parser.processData(&buff, &packet)) {
		CLOG(WARNING) << "Parse http packet failed.";
		break;
	    }
	    CLOG(INFO) << "Reveive:" << packet.getURI();
	    /* Check if it is valid http request.*/
	    if (!validatePacket(packet)) {
		CLOG(WARNING) << "validate http packet failed.";		
		break;
	    }
	    HttpResponsePacket resp;
	    _handler->process(packet, resp);
	    if (!writeData(sk, resp)) {
		CLOG(ERROR) << "Send response failed.";
		break;
	    }
	    CLOG(INFO) << "Send response success.";
	} while(0);
	sk->shutdown();
    }

    bool SocketProcessor::validatePacket(HTTPPacket &packet) const {
	if (!_needCheck) {
	    return true;
	}
	return RequestValidator::validate(packet, _secret);
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
