#include "timer.h"

BEGIN_NAMESPACE(cgserver)

Timer::Timer(asio_service &service):_service(service){
    
}

Timer::~Timer(){
}

void Timer::makeServiceBusy() {
    _timer.reset(new deadlineTimer(_service, boost::posix_time::seconds(24*60*60)));
    _timer->async_wait(std::bind(&Timer::timerLooper, this, std::placeholders::_1));
}

void Timer::timerLooper(const asio_error &err) {
    if (err) {
	CLOG(WARNING) << "Meet error in timer: " << err;
    }
    _timer->async_wait(std::bind(&Timer::timerLooper, this, std::placeholders::_1));    
}

END_NAMESPACE
