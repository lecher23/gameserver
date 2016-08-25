#include "servertimer.h"

BEGIN_NAMESPACE(cgserver)

ServerTimer::ServerTimer(asio_service &s): _service(s){
    _timers[EET_10_S].reset(
	new asio_deadline_timer(_service, asio_seconds(10)));
    _timers[EET_1_M].reset(
	new asio_deadline_timer(_service, asio_seconds(60)));
    _timers[EET_5_M].reset(
	new asio_deadline_timer(_service, asio_seconds(300)));
    _timers[EET_24_H].reset(
	new asio_deadline_timer(_service, asio_seconds(24*60*60)));
}

ServerTimer::~ServerTimer(){
}

void ServerTimer::funcWrapper(std::function<void(void)> func, const asio_error &err) {
    if (err) {
	CLOG(WARNING) << "Timer error:" << err;
	return;
    }
    func();
}

void ServerTimer::addTask(std::function<void(void)> func, ExpireType t) {
    if(_timers[t].get() != NULL) {
	_timers[t]->async_wait(
	    std::bind(&ServerTimer::funcWrapper, this, func, std::placeholders::_1));
    }
}

void ServerTimer::addTask(TimerExecutorPtr timer, ExpireType t) {
    if(_timers[t].get() != NULL) {
	_timers[t]->async_wait(
	    std::bind(&ServerTimer::executorWrapper, this, timer, std::placeholders::_1));
    }
}

void ServerTimer::executorWrapper(TimerExecutorPtr timer, const asio_error &err){
    if (err) {
	CLOG(WARNING) << "Timer error:" << err;
	return;
    }
    timer->execute();
}

END_NAMESPACE
