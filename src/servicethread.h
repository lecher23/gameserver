#ifndef SERVICETHREAD_H
#define SERVICETHREAD_H

#include <util/common_define.h>

BEGIN_NAMESPACE(cgserver)

class ServiceThread{
 public:
    ServiceThread(asio_service &service);
    ~ServiceThread();

    bool start();
    void stop(bool force = false);
    void run();
    bool isStart() { return _started;}
    
 private:
    asio_service &_service;
    asio_work_ptr _work;
    std::shared_ptr<std::thread> _t;
    asio_error _err;
    bool _started;
};

DF_SHARED_PTR(ServiceThread)

END_NAMESPACE
#endif
