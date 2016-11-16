#ifndef RESPONSEQUEUE_H
#define RESPONSEQUEUE_H

#include "util/common_define.h"

BEGIN_NAMESPACE(cgserver)

template <typename T>
class ResponseQueue{
public:
    ResponseQueue(int capacity = 64) {
        _members.resize(capacity);
        _capacity = capacity;
    }

    ~ResponseQueue() {
    }

    int32_t empty() {
        return _start == _end;
    }

#define CIRCULAR_INCR(val, delimiter)           \
    if (++val == delimiter) {                   \
        val = 0;                                \
    }

    void push_back(const T &t) {
        MUTEX_GUARD(_lock);
        _members[_end] = std::move(t);
        CIRCULAR_INCR(_end, _capacity);
        if (_end == _start) {
            // consume last element
            ++_start;
        }
    }

    void pop() {
        CIRCULAR_INCR(_start, _capacity);
    }

    const T &front() {
        MUTEX_GUARD(_lock);
        if (empty()) {
            return "";
        }
        return _members[_start];
    }

    T pop_front() {
        MUTEX_GUARD(_lock);
        if (empty()) {
            return "";
        }
        int32_t tmp = _start;
        CIRCULAR_INCR(_start, _capacity);
        return _members[tmp];
    }
#undef CIRCULAR_INCR

private:
    int32_t _capacity;
    int32_t _start{0};
    int32_t _end{0};
    std::vector<T> _members;
    std::mutex _lock;
};

END_NAMESPACE
#endif
