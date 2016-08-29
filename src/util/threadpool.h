#ifndef __UTIL_THREAD_POOL_H_
#define __UTIL_THREAD_POOL_H_

#include "thread.h"
#include "threadlock.h"
#include "scopedlock.h"
#include "circularqueue.h"
#include <vector>
#include <pthread.h>
#include <util/common_define.h>

namespace cgserver 
{

class ThreadPool
{
public:
    enum 
    {
        DEFAULT_THREADNUM = 4,
        DEFAULT_QUEUESIZE = 512
    };

    enum STOP_TYPE 
    {
        STOP_THREAD_ONLY = 0,
        STOP_AND_CLEAR_QUEUE,
        STOP_AFTER_QUEUE_EMPYT
    };
    
    enum ERROR_TYPE 
    {
        ERROR_NONE = 0,
        ERROR_POOL_HAS_STOP,
        ERROR_POOL_ITEM_IS_NULL,
        ERROR_POOL_QUEUE_FULL,
    };

    enum task_queue_status_t
    {
	task_queue_normal,
	task_queue_busy
    };
public:
    ThreadPool(const size_t threadNum = DEFAULT_THREADNUM,
               const size_t queueSize = DEFAULT_QUEUESIZE,
               uint32_t stack_size=0);
    bool initBusyCheck(const uint32_t task_queue_busy_check_window_size,
                        const uint32_t abnormal_task_queue_size,
                        const uint32_t sap_abnormal_status_threshold);
    ~ThreadPool();
    
public:
    size_t getTaskCount() const;
    size_t getRestartTaskCount() const;
    inline size_t getThreadNum() const {return _threadNum;}
    inline size_t getMaxQueueSize() const {return _maxQueueSize;}
    inline uint32_t getStackSize() const {return _stack_size;}
    bool start();
    void stop(STOP_TYPE stopType = STOP_AFTER_QUEUE_EMPYT);
    void clearQueue();
    ERROR_TYPE pushTask(std::function<void()> f, bool isBlocked = false);
    void pushRestartTask(std::function<void()> f);
    task_queue_status_t getTaskQueueStatus();
private:
    ThreadPool(const ThreadPool&);
    ThreadPool& operator = (const ThreadPool&);
private:
    bool createThreads();
    pthread_t createThread(uint32_t idx, uint32_t stack_size);
    static void *workerLoop(void *args);
    void waitQueueEmpty();
    void stopThread();
private:
    size_t _threadNum;
    size_t _maxQueueSize;
    CircularQueue<std::function<void()> > _queue;
    CircularQueue<std::function<void()> > _restart_queue;
    std::vector<pthread_t> _threads;
    mutable ProducerConsumerCond _cond;
    volatile bool _push;
    volatile bool _run;
    uint32_t _stack_size;

    uint32_t _abnormal_task_queue_size;
    uint32_t _sap_abnormal_status_threshold;
    uint32_t _abnormal_status_num;
    CircularQueue<bool> _queue_status_window;

public:
    bool _enable_busy_check;
};

 DF_SHARED_PTR(ThreadPool);
 
}

#endif //AUTIL_THREADPOOL_H
