#include "task.h"
#include <iostream>

namespace cgserver{
    Task::Task(SocketPtr socket, IProcessorPtr processor):
	_socket(socket), _processor(processor)
    {
    }
    
    Task::~Task(){
    }

    void Task::run(Thread *t, void *args) {
	if (_processor.get() == NULL || _socket.get() == NULL) {
	    return;
	}
	// processor will do left.
	_processor->process(_socket);
    }
    
    void Task::free() {
	// release it self
	delete this;
    }
}
