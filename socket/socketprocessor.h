#ifndef SOCKET_PROCESSOR_H
#define SOCKET_PROCESSOR_H
#include "../util/runnable.h"
#include "../util/thread.h"
#include "../util/databuffer.h"
#include "../http/httpresponsepacket.h"
#include "../handlers/ihandler.h"
#include "socket.h"

namespace cgserver{
#define MAX_BUFF_SIZE 4096    
    class SocketProcessor:Runnable {
    public:
	SocketProcessor();
	virtual ~SocketProcessor();
	virtual void run(Thread *thread, void *arg);
	
	void initResource(IHandler *handler);
    private:
	bool readData(Socket *sk, DataBuffer &buff);
	bool writeData(Socket *sk, HttpResponsePacket &resp);
	IHandler *_handler; 
    };
}

#endif
