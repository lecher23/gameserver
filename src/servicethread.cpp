#include "servicethread.h"

BEGIN_NAMESPACE(cgserver)

ServiceThread::ServiceThread(asio_service &service):_service(service), _started(false){
}

ServiceThread::~ServiceThread(){
}

void ServiceThread::run() {
    _work.reset(new asio_work(_service));
    _started = true;
    _service.run(_err);
    if (_err) {
	CLOG(ERROR) << "start io service failed:" << _err;
	_work.reset();
    }
    _started = false;
}

bool ServiceThread::start() {
    _t.reset(new std::thread(std::bind(&ServiceThread::run, this)));
    return _err == 0;
}

void ServiceThread::stop(bool force) {
    if (!_started)
	return;
    if (force){
	_service.stop();
    } else {
	_work.reset();
    }
    _t->join();
}

END_NAMESPACE
