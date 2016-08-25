#ifndef SERVERTIMER_H
#define SERVERTIMER_H

#include <util/common_define.h>
#include <server/timerexecutor.h>

BEGIN_NAMESPACE(cgserver)

class ServerTimer{
 public:
    enum ExpireType{
	EET_10_S,
	EET_1_M,
	EET_5_M,
	EET_24_H,
    };
    ServerTimer(asio_service &service);
    ~ServerTimer();

    void addTask(std::function<void(void)> func, ExpireType t);
    void addTask(TimerExecutorPtr timer, ExpireType t);

    void funcWrapper(std::function<void(void)> func, const asio_error &err);
    void executorWrapper(TimerExecutorPtr timer, const asio_error &err);
 private:
    asio_service &_service;
    std::map<ExpireType, asio_deadline_timer_ptr> _timers;
};
DF_SHARED_PTR(ServerTimer);

END_NAMESPACE
#endif
