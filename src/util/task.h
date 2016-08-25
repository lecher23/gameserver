#ifndef TASK_H
#define TASK_H

#include "runnable.h"
#include "../socket/socket.h"
#include "iprocessor.h"

namespace cgserver{
    class Task:Runnable{
    public:
        Task(SocketPtr socket, IProcessorPtr processor);
        virtual ~Task();
	virtual void run(Thread *thread, void *arg);
	virtual void free();

    private :
	SocketPtr _socket;
	IProcessorPtr _processor;
    };
}
#endif
