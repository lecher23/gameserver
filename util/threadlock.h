#ifndef _UTIL_THREAD_LOCK_H_
#define _UTIL_THREAD_LOCK_H_


#include <pthread.h>
#include <stdint.h>
#include "timeutil.h"
#include "common_define.h"

#ifndef __USE_PTHREAD_RWLOCK
#  if defined __USE_UNIX98 || defined __USE_XOPEN2K
#    define __USE_PTHREAD_RWLOCK
#  endif
#endif //__USE_PTHREAD_RWLOCK

namespace cgserver {
#define COPY_CONSTRUCTOR(T) T(const T &);

class ThreadMutex
{
public:
	ThreadMutex();
	~ThreadMutex();
private:
	COPY_CONSTRUCTOR(ThreadMutex);
public:
	common::ret_t lock();
	common::ret_t trylock();
	common::ret_t unlock();
protected:
	pthread_mutex_t _mutex;
};


class RWLock
{
public:
	RWLock();
	~RWLock();
private:
	COPY_CONSTRUCTOR(RWLock);
public:
	common::ret_t rdlock();
	common::ret_t wrlock();
	common::ret_t tryrdlock();
	common::ret_t trywrlock();
	common::ret_t unlock();
private:
#ifdef __USE_PTHREAD_RWLOCK
	pthread_rwlock_t _inst;
#else //__USE_PTHREAD_RWLOCK
	pthread_mutex_t _inst;
#endif //__USE_PTHREAD_RWLOCK
};

class Condition
{
public:
	Condition();
	~Condition();
private:
	COPY_CONSTRUCTOR(Condition);
public:
	common::ret_t lock();
	common::ret_t unlock();
	common::ret_t wait();
	common::ret_t timedwait(uint32_t ms);
	common::ret_t signal();
	common::ret_t broadcast();
private:
	pthread_mutex_t _lock;
	pthread_cond_t _cond;
};

class Conditions
{
public:
	Conditions(uint32_t n);
	~Conditions();
private:
	COPY_CONSTRUCTOR(Conditions);
public:
	common::ret_t lock();
	common::ret_t unlock();
	common::ret_t wait(uint32_t idx);
	common::ret_t timedwait(uint32_t idx, uint32_t ms);
	common::ret_t signal(uint32_t idx);
	common::ret_t broadcast(uint32_t idx);
private:
	uint32_t _unCount;
	pthread_cond_t * _conds;
	pthread_mutex_t _lock;
};

class BoolCondition
{
public:
	BoolCondition();
	~BoolCondition();
private:
	COPY_CONSTRUCTOR(BoolCondition);
public:
	common::ret_t setBusy();
	common::ret_t waitIdle();
	bool isBusy();
	common::ret_t clearBusy();
	common::ret_t clearBusyBroadcast();
private:
	bool _bBusy;
	Condition _cond;
};

class ProducerConsumerCond : public ThreadMutex
{
public:
    ProducerConsumerCond()
    {
        pthread_cond_init(&_producerCond, NULL);
        pthread_cond_init(&_consumerCond, NULL);
    }

    ~ProducerConsumerCond()
    {
        pthread_cond_destroy(&_producerCond);
        pthread_cond_destroy(&_consumerCond);
    }
public:
    int producerWait(int64_t usec = 0) {
        return wait(_producerCond, usec);
    }

    int signalProducer() {
        return signal(_producerCond);
    }

    int broadcastProducer() {
        return broadcast(_producerCond);
    }

    int consumerWait(int64_t usec = 0) {
        return wait(_consumerCond, usec);
    }

    int signalConsumer() {
        return signal(_consumerCond);
    }

    int broadcastConsumer() {
        return broadcast(_consumerCond);
    }

private:
    int wait(pthread_cond_t &cond, int64_t usec)
    {
        int ret = 0;
        if (usec <= 0) {
            ret = pthread_cond_wait(&cond, &_mutex);
        } else {
            timespec ts = CTimeUtil::getTimespec(usec);
            ret = pthread_cond_timedwait(&cond, &_mutex, &ts);
        }

        return ret;
    }

    static int signal(pthread_cond_t &cond) {
        return pthread_cond_signal(&cond);
    }

    static int broadcast(pthread_cond_t &cond) {
        return pthread_cond_broadcast(&cond);
    }

protected:
    pthread_cond_t  _producerCond;
    pthread_cond_t  _consumerCond;
};


typedef ThreadMutex CThreadMutex;
typedef RWLock CThreadRWLock;
typedef Condition CThreadCond;
typedef Conditions CThreadConds;

}

#endif //_UTIL_THREAD_LOCK_H_
