#ifndef TIMER_H
#define TIMER_H

#include <util/common_define.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

BEGIN_NAMESPACE(cgserver)

typedef boost::asio::io_service asio_service;
typedef boost::asio::deadline_timer deadlineTimer;
typedef boost::system::error_code asio_error;
DF_SHARED_PTR(deadlineTimer);

class Timer{
 public:
    Timer(asio_service &service);
    ~Timer();

    void makeServiceBusy();
    
    void timerLooper(const asio_error &err);
 private:
    boost::asio::io_service &_service;
    deadlineTimerPtr _timer;
};

END_NAMESPACE
#endif
