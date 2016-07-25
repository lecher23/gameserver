#include "threadlock.h"
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/time.h>

namespace cgserver {

//================================== Mutex =========================================//
ThreadMutex::ThreadMutex()
{
    int ret = pthread_mutex_init(&_mutex, NULL);
    (void)ret;
    assert(ret == 0);
}

ThreadMutex::~ThreadMutex()
{
    int ret = pthread_mutex_destroy(&_mutex);
    (void)ret;
    assert(ret == 0);
}

common::ret_t ThreadMutex::lock()
{
    return (pthread_mutex_lock(&_mutex) == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t ThreadMutex::unlock()
{
    return (pthread_mutex_unlock(&_mutex) == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t ThreadMutex::trylock()
{
    int ret = pthread_mutex_trylock(&_mutex);
    if( ret == EBUSY ) {
        return common::r_eagain;
    }
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}



//================================== RWLock =========================================//
RWLock::RWLock() {
#ifdef __USE_PTHREAD_RWLOCK
    int ret = pthread_rwlock_init(&_inst, NULL);
#else //__USE_PTHREAD_RWLOCK
    int ret = pthread_mutex_init(&_inst, NULL);
#endif //__USE_PTHREAD_RWLOCK
    (void)ret;
    assert(ret == 0);
}

RWLock::~RWLock() {
#ifdef __USE_PTHREAD_RWLOCK
    int ret = pthread_rwlock_destroy(&_inst);
#else //__USE_PTHREAD_RWLOCK
    int ret = pthread_mutex_destroy(&_inst);
#endif //__USE_PTHREAD_RWLOCK
    (void)ret;
    assert(ret == 0);
}

common::ret_t RWLock::rdlock() {
#ifdef __USE_PTHREAD_RWLOCK
    int ret = pthread_rwlock_rdlock(&_inst);
#else //__USE_PTHREAD_RWLOCK
    int ret = pthread_mutex_lock(&_inst);
#endif //__USE_PTHREAD_RWLOCK
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t RWLock::wrlock() {
#ifdef __USE_PTHREAD_RWLOCK
    int ret = pthread_rwlock_wrlock(&_inst);
#else //__USE_PTHREAD_RWLOCK
    int ret = pthread_mutex_lock(&_inst);
#endif //__USE_PTHREAD_RWLOCK
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t RWLock::tryrdlock() {
#ifdef __USE_PTHREAD_RWLOCK
    int ret = pthread_rwlock_tryrdlock(&_inst);
#else //__USE_PTHREAD_RWLOCK
    int ret = pthread_mutex_trylock(&_inst);
#endif //__USE_PTHREAD_RWLOCK
    if( ret == EBUSY ) {
        return common::r_eagain;
    }
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t RWLock::trywrlock() {
#ifdef __USE_PTHREAD_RWLOCK
    int ret = pthread_rwlock_trywrlock(&_inst);
#else //__USE_PTHREAD_RWLOCK
    int ret = pthread_mutex_trylock(&_inst);
#endif //__USE_PTHREAD_RWLOCK
    if( ret == EBUSY ) {
        return common::r_eagain;
    }
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t RWLock::unlock() {
#ifdef __USE_PTHREAD_RWLOCK
    int ret = pthread_rwlock_unlock(&_inst);
#else //__USE_PTHREAD_RWLOCK
    int ret = pthread_mutex_unlock(&_inst);
#endif //__USE_PTHREAD_RWLOCK
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}


//================================== Condition =========================================//
Condition::Condition() {
    int ret = pthread_mutex_init(&_lock, NULL);
    assert(ret == 0);
    ret = pthread_cond_init(&_cond, NULL);
    assert(ret == 0);
}

Condition::~Condition() {
    int ret = pthread_cond_destroy(&_cond);
    assert(ret == 0);
    ret = pthread_mutex_destroy(&_lock);
    assert(ret == 0);
}

common::ret_t Condition::lock() {
    return (pthread_mutex_lock(&_lock) == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t Condition::unlock() {
    return (pthread_mutex_unlock(&_lock) == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t Condition::wait() {
    return (pthread_cond_wait(&_cond, &_lock) == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t Condition::timedwait(uint32_t ms) {
    if( ms == 0 ) {
        return wait();
    }
    struct timespec tim;
    struct timeval  tv;
    gettimeofday(&tv , 0);
    tim.tv_sec = ms / 1000 + tv.tv_sec + (ms % 1000 + tv.tv_usec / 1000) / 1000;
    tim.tv_nsec = (ms % 1000 * 1000 + tv.tv_usec) % 1000000 * 1000;
    int ret = pthread_cond_timedwait(&_cond, &_lock, &tim);
    if( ret == ETIMEDOUT ) {
        return common::r_eagain;
    }
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t Condition::signal() {
    return (pthread_cond_signal(&_cond) ? common::r_succeed : common::r_epthread);
}

common::ret_t Condition::broadcast() {
    return (pthread_cond_broadcast(&_cond) ? common::r_succeed : common::r_epthread);
}


//================================== Conditions =========================================//
Conditions::Conditions(uint32_t n):_unCount(n), _conds(NULL) {
    assert(n > 0);
    _conds = (pthread_cond_t *)malloc(sizeof(pthread_cond_t) * n);
    assert(_conds != NULL);
    int r;
    for( uint32_t i = 0; i < n; i++ ) {
        r = pthread_cond_init(&_conds[i], NULL);
        if( r != 0 ) {
            for( uint32_t j = 0; j < i; j++ ) {
                pthread_cond_destroy(&_conds[j]);
            }
            free(_conds);
            _conds = NULL;
        }
        assert(r == 0);
    }
    r = pthread_mutex_init(&_lock, NULL);
    if( r != 0 ) {
        for( uint32_t j = 0; j < n; j++ ) {
            pthread_cond_destroy(&_conds[j]);
        }
        free(_conds);
        _conds = NULL;
    }
    assert(r == 0);
}

Conditions::~Conditions() {
    int r;
    r = pthread_mutex_destroy(&_lock);
    assert(r == 0);
    if( _conds != NULL ) {
        for( uint32_t i = 0; i < _unCount; i++ ) {
            int ret = pthread_cond_destroy(&_conds[i]);
            if( ret != 0 ) r = ret;
        }
        free(_conds);
        _conds = NULL;
        assert(r == 0);
    }
}

common::ret_t Conditions::lock() {
    return (pthread_mutex_lock(&_lock) ? common::r_succeed : common::r_epthread);
}

common::ret_t Conditions::unlock() {
    return (pthread_mutex_unlock(&_lock) ? common::r_succeed : common::r_epthread);
}

common::ret_t Conditions::wait(uint32_t idx) {
    if(idx >= _unCount) return common::r_outofbounds;
    return (pthread_cond_wait(&_conds[idx], &_lock) ? common::r_succeed : common::r_epthread);
}

common::ret_t Conditions::timedwait(uint32_t idx, uint32_t ms) {
    if(idx >= _unCount) return common::r_outofbounds;
    if( ms == 0 ) {
        return wait(idx);
    }
    struct timespec tim;
    struct timeval  tv;
    gettimeofday(&tv , 0);
    tim.tv_sec = ms / 1000 + tv.tv_sec + (ms % 1000 + tv.tv_usec / 1000) / 1000;
    tim.tv_nsec = (ms % 1000 * 1000 + tv.tv_usec) % 1000000 * 1000;
    int ret = pthread_cond_timedwait(&_conds[idx], &_lock, &tim);
    if( ret == ETIMEDOUT ) {
        return common::r_eagain;
    }
    return (ret == 0 ? common::r_succeed : common::r_epthread);
}

common::ret_t Conditions::signal(uint32_t idx) {
    if( idx >= _unCount ) return common::r_outofbounds;
    return (pthread_cond_signal(&_conds[idx]) ? common::r_succeed : common::r_epthread);
}

common::ret_t Conditions::broadcast(uint32_t idx) {
    if(idx >= _unCount) return common::r_outofbounds;
    return (pthread_cond_broadcast(&_conds[idx]) ? common::r_succeed : common::r_epthread);
}


//================================== BoolCondition =========================================//
BoolCondition::BoolCondition() : _bBusy(false) {
}

BoolCondition::~BoolCondition() {
}

common::ret_t BoolCondition::setBusy() {
    common::ret_t ret;
    ret = _cond.lock();
    if( ret != common::r_succeed) return ret;
    while( _bBusy ) {
        ret = _cond.wait();
        if( ret != common::r_succeed ) {
            _cond.unlock();
            return ret;
        }
    }
    _bBusy = true;
    return _cond.unlock();
}

common::ret_t BoolCondition::waitIdle() {
    common::ret_t ret;
    ret = _cond.lock();
    if( ret != common::r_succeed) return ret;
    while( _bBusy ) {
        ret = _cond.wait();
        if( ret != common::r_succeed ) {
            _cond.unlock();
            return ret;
        }
    }
    return _cond.unlock();
}

bool BoolCondition::isBusy() {
    bool r;
    _cond.lock();
    r = _bBusy;
    _cond.unlock();
    return r;
}

common::ret_t BoolCondition::clearBusy() {
    common::ret_t ret;
    ret = _cond.lock();
    if( ret != common::r_succeed ) return ret;
    _bBusy = false;
    ret = _cond.signal();
    if( ret != common::r_succeed ) {
        _cond.unlock();
        return ret;
    }
    return _cond.unlock();
}

common::ret_t BoolCondition::clearBusyBroadcast() {
    common::ret_t ret;
    ret = _cond.lock();
    if( ret != common::r_succeed ) return ret;
    _bBusy = false;
    ret = _cond.broadcast();
    if( ret != common::r_succeed ) {
        _cond.unlock();
        return ret;
    }
    return _cond.unlock();
}

}
