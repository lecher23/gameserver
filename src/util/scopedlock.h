#ifndef _UTIL_SCOPED_LOCK_H_
#define _UTIL_SCOPED_LOCK_H_
#include "threadlock.h"

#define MUTEX_LOCK(x) { ScopedMutex __scoped_mutex_##x(x);
#define MUTEX_UNLOCK(x) }

#define RD_LOCK(x) { ScopedRdLock __scoped_rd_##x(x);
#define RD_UNLOCK(x) }

#define WR_LOCK(x) { ScopedWrLock __scoped_wr_##x(x);
#define WR_UNLOCK(x) }

#define COND_LOCK(x) { ScopedCondition __scoped_cond_##x(x);
#define COND_UNLOCK(x) }

namespace cgserver {

class ScopedMutex
{
public:
    inline ScopedMutex(ThreadMutex & lock) : _lock(lock) {
        _lock.lock();
    }
    inline ~ScopedMutex() {
        _lock.unlock();
    }
private:
    ThreadMutex & _lock;
};

class ScopedRdLock
{
public:
    inline ScopedRdLock(RWLock & lock) : _lock(lock) {
        _lock.rdlock();
    }
    inline ~ScopedRdLock() {
        _lock.unlock();
    }
private:
    RWLock & _lock;
};

class ScopedWrLock
{
public:
    inline ScopedWrLock(RWLock & lock) : _lock(lock) {
        _lock.wrlock();
    }
    inline ~ScopedWrLock() {
        _lock.unlock();
    }
private:
    RWLock & _lock;
};

class ScopedCondition
{
public:
    inline ScopedCondition(Condition & lock) : _lock(lock) {
        _lock.lock();
    }
    inline ~ScopedCondition() {
        _lock.unlock();
    }
private:
    Condition & _lock;
};

}

#endif //_UTIL_SCOPED_LOCK_H_
