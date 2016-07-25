#include "task.h"
#include <iostream>

namespace cgserver{
    Task::Task(Socket *socket, IProcessor *processor):
	_socket(socket), _processor(processor)
    {
    }
    
    Task::~Task(){
	if (_socket != NULL) {
	    delete _socket;
	    _socket = NULL;
	}
    }

    void Task::run(Thread *t, void *args) {
	if (_processor == NULL || socket == NULL) {
	    return;
	}
	// processor will do left.
	_processor->process(_socket);
    }
    
    void Task::free() {
	// release it self
	delete this;
	// _processor should delete outside.
    }
}
