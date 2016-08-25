#include <resourcefactory.h>

BEGIN_NAMESPACE(cgserver)

ResourceFactory::ResourceFactory(){
}

ResourceFactory::~ResourceFactory(){
    if (_p_service_thread.get() != NULL && _p_service_thread->isStart()) {
	_p_service_thread->stop(true);
    }
}

bool ResourceFactory::init() {
    return true;
}

ServiceThreadPtr ResourceFactory::getServiceThread() {
    if (_p_service_thread.get() == NULL) {
	_p_service_thread.reset(new ServiceThread(_service));
    }
    return _p_service_thread;
}

ServerTimerPtr ResourceFactory::getServerTimer() {
    if (_p_server_timer.get() == NULL) {
	_p_server_timer.reset(new ServerTimer(_service));
    }
    return _p_server_timer;
}

END_NAMESPACE
