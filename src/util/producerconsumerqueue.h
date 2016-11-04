#ifndef PRODUCERCONSUMERQUEUE_H
#define PRODUCERCONSUMERQUEUE_H

#include "circularqueue.h"
#include "common_define.h"
#include <condition_variable>

BEGIN_NAMESPACE(cgserver)

template <typename T>
class ProducerConsumerQueue{
public:
    ProducerConsumerQueue() {}
    ~ProducerConsumerQueue() {}

    void setCapacity(size_t capacity){
        _queue.set_capacity(capacity);
    }

    void produce(const T &item) {
        _queueLock.lock();
        if (_queue.size() == _queue.capacity()) {
            // remove head;
            CLOG(WARNING) << "Too many product, remove head product.";
        }
        _queue.push_back(item);
        _queueLock.unlock();
        _dataReady.notify_all();
    }

    const T &consume() {
        std::unique_lock<std::mutex> lock(_queueLock);
        while (_queue.empty()) {
            _dataReady.wait(lock);
        }
        const auto &data = _queue.front();
        _queue.pop_front();
        lock.unlock();
        return data;
    }

    bool empty() {
        MUTEX_GUARD(_queueLock);
        return _queue.empty();
    }

    bool size() {
        return _queue.size();
    }

private:
    std::mutex _queueLock;
    std::condition_variable _dataReady;
    CircularQueue<T> _queue;
};

END_NAMESPACE
#endif
