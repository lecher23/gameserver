#ifndef TASK_H
#define TASK_H

#include "runnable.h"
#include "../socket/socket.h"
#include "iprocessor.h"

namespace cgserver{
    class Task:Runnable{
    public:
        Task(Socket *socket, IProcessor *processor);
        virtual ~Task();
	virtual void run(Thread *thread, void *arg);
	virtual void free();

    private :
	Socket *_socket;
	IProcessor *_processor;
    };
}
#endif
