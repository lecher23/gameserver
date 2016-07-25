#ifndef SOCKET_PROCESSOR_H
#define SOCKET_PROCESSOR_H
#include "../util/runnable.h"
#include "../util/thread.h"
#include "../util/databuffer.h"
#include "../util/iprocessor.h"
#include "../http/httpresponsepacket.h"
#include "../handlers/ihandler.h"
#include "socket.h"

namespace cgserver{
#define MAX_BUFF_SIZE 4096    
    class SocketProcessor:IProcessor {
    public:
	SocketProcessor(IHandler *handler);
	virtual ~SocketProcessor();
	
	virtual void process(Socket *clientSocket) const;
	
    private:
	bool readData(Socket *sk, DataBuffer &buff) const ;
	bool writeData(Socket *sk, HttpResponsePacket &resp) const ;

	IHandler *_handler;	
    };
}

#endif
