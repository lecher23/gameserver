#ifndef SOCKET_PROCESSOR_H
#define SOCKET_PROCESSOR_H
#include "runnable.h"
#include "thread.h"
#include "databuffer.h"
#include "httpresponsepacket.h"
#include "ihandler.h"

namespace cgserver{
#define MAX_BUFF_SIZE 4096    
    class SocketProcessor:Runnable {
    public:
	SocketProcessor();
	virtual ~SocketProcessor();
	virtual void run(Thread *thread, void *arg);
	
	void initResource(IHandler *handler);
    private:
	void readData(int fd, DataBuffer &buff);
	void writeData(int fd, HttpResponsePacket &resp);
	IHandler *_handler; 
    };
}

#endif
