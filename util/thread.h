#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>
#include <memory>
#include "runnable.h"

namespace cgserver {
  class Thread {

  public:
    /**
     * 构造函数
     */
    Thread() {
    	tid = 0;
    }

    /**
     * 起一个线程，开始运行
     */
    void start(Runnable *r, void *a) {
    	runnable = r;
    	args = a;
    	pthread_create(&tid, NULL, Thread::hook, this);
    }

    /**
     * 等待线程退出
     */
    void join() {
    	if (tid) {
    		pthread_join(tid, NULL);
    	}
    	tid = 0;
    }

    /**
     * 得到Runnable对象
     * 
     * @return Runnable
     */
    Runnable *getRunnable() {
    	return runnable;
    }
    
    /**
     * set the priority of this thread
     * @param priority Note only process with root privilege can set
     * priority to value greater than 0.
     * @param policy scheduling policy. Valid values are SCHED_OTHER,
     * SCHED_FIFO, SCHED_RR. SCHED_OTHER only has one priority 0.
     * SCHED_FIFO and SCHED_RR have priorities ranges from 1 to 99.
     * @return return true if success. 
     **/
    bool setPriority(int priority, int policy = SCHED_OTHER) {
    	struct sched_param schedparam;
    	schedparam.sched_priority = priority;
    	int ret = pthread_setschedparam(tid, policy, &schedparam);
    	if(ret != 0) {
    		return false;
    	}
    	return true;
    }

    /**
     * 得到回调参数
     * 
     * @return args
     */
    void *getArgs() {
    	return args;
    }

    /**
     * 线程的回调函数
     * 
     */

    static void *hook(void *arg) {
    	Thread *thread = (Thread*) arg;

    	if (thread->getRunnable()) {
    		thread->getRunnable()->run(thread, thread->getArgs());
    	}

    	return (void*) NULL;
    }

  private:

    	pthread_t tid;
    	Runnable *runnable;
    	void *args;
  };
  typedef std::shared_ptr<Thread> ThreadPtr;

}

#endif /*THREAD_H_*/
