#ifndef RESOURCEFACTORY_H
#define RESOURCEFACTORY_H

#include <util/common_define.h>
#include <servicethread.h>
#include <servertimer.h>

BEGIN_NAMESPACE(cgserver)

typedef std::function<void(void)> TimerFunc;

class ResourceFactory{
 public:
    ~ResourceFactory();

    static ResourceFactory &getInstance() {
	static ResourceFactory p;
	return p;
    }
    
    asio_service &getIoService() {
	return _service;
    }

    ServiceThreadPtr getServiceThread();
    ServerTimerPtr getServerTimer();
    bool init();
 private:
    ResourceFactory();
    ResourceFactory(const ResourceFactory&);
    asio_service _service;
    asio_work_ptr _work;
    ServiceThreadPtr _p_service_thread;
    ServerTimerPtr _p_server_timer;
};

END_NAMESPACE
#endif
