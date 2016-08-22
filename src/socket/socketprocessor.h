#ifndef SOCKET_PROCESSOR_H
#define SOCKET_PROCESSOR_H
#include "../util/runnable.h"
#include "../util/thread.h"
#include "../util/databuffer.h"
#include "../util/iprocessor.h"
#include "../handlers/ihandler.h"
#include "../http/httpresponsepacket.h"
#include "socket.h"

namespace cgserver{
#define MAX_BUFF_SIZE 4096    
    class SocketProcessor:IProcessor {
    public:
	SocketProcessor();
	virtual ~SocketProcessor();
	
	virtual void process(SocketPtr clientSocket) const;
	virtual bool init(void *handler);
	
    private:
	bool readData(SocketPtr &sk, DataBuffer &buff) const ;
	bool writeData(SocketPtr &sk, HttpResponsePacket &resp) const ;
	bool validatePacket(HTTPPacket &packet) const;	

	IHandler *_handler;
	bool _needCheck;
	std::string _secret;
    };
}

#endif
