#include "threadpool.h"
#include <unistd.h>

using namespace std;

BEGIN_NAMESPACE(cgserver)

ThreadPool::ThreadPool(const size_t threadNum, const size_t queueSize)
    : _threadNum(threadNum)
    , _maxQueueSize(queueSize)
    , _queue(queueSize ? queueSize : DEFAULT_QUEUESIZE)
    , _restart_queue(queueSize ? queueSize + 1000: DEFAULT_QUEUESIZE)
    , _push(true)
    , _run(false)
    , _enable_busy_check(false)
{
    if (_threadNum == 0) {
        _threadNum = DEFAULT_THREADNUM;
    }
    if (_maxQueueSize == 0) {
        _maxQueueSize = DEFAULT_QUEUESIZE;
    }
}

ThreadPool::~ThreadPool() 
{
    stop(STOP_AND_CLEAR_QUEUE);
}

bool ThreadPool::initBusyCheck(const uint32_t task_queue_busy_check_window_size,
                                const uint32_t abnormal_task_queue_size,
                                const uint32_t sap_abnormal_status_threshold)
{
    _enable_busy_check = true;
    _queue_status_window.set_capacity(task_queue_busy_check_window_size);
    _abnormal_task_queue_size = abnormal_task_queue_size;
    _sap_abnormal_status_threshold = sap_abnormal_status_threshold;
    _abnormal_status_num = 0;
    return true;
}

size_t ThreadPool::getTaskCount() const
{
    ScopedMutex lock(_cond);
    return _queue.size();
}

size_t ThreadPool::getRestartTaskCount() const {
    ScopedMutex lock(_cond);
    return _restart_queue.size();
}

ThreadPool::task_queue_status_t ThreadPool::getTaskQueueStatus()
{
    if (_enable_busy_check &&
        _abnormal_status_num >= _sap_abnormal_status_threshold)
    {
        return task_queue_busy;
    }

    return task_queue_normal;
}

ThreadPool::ERROR_TYPE ThreadPool::pushTask(std::function<void()> f, bool isBlocked) 
{
    if (!_push)
    {
        return ERROR_POOL_HAS_STOP;
    }

    ScopedMutex lock(_cond);
    if (_enable_busy_check) {
        uint32_t task_queue_size = _queue.size();
        if (_queue_status_window.size() == _queue_status_window.capacity())
        {
            if (_queue_status_window.front() == true)
            {
                _abnormal_status_num -= 1;
            }
            _queue_status_window.pop_front();
        }
        if (task_queue_size >= _abnormal_task_queue_size)
        {
            _queue_status_window.push_back(true);
            _abnormal_status_num += 1;
        }else {
            _queue_status_window.push_back(false);
        }
    }

    // none block just return error
    if (!isBlocked && _queue.size() >= _maxQueueSize)
    {
        return ERROR_POOL_QUEUE_FULL;
    }

    // block will wait until queue not empty
    while (_push && _queue.size() >= _maxQueueSize)
    {
        _cond.producerWait();
    }

    if (!_push)
    {
        return ERROR_POOL_HAS_STOP;
    }
    // add runner ok
    _queue.push_back(f);
    _cond.signalConsumer();
    return ERROR_NONE;
}

void ThreadPool::pushRestartTask(std::function<void()> f) 
{
    ScopedMutex lock(_cond);
    _restart_queue.push_back(f);
    _cond.signalConsumer();
    return;
}

bool ThreadPool::start() 
{
    CLOG(INFO) << "Start thread poll";
    if (_run)
    {
        //("Already started!");
        return false;
    }

    _push = true;
    _run = true;
 
    if (createThreads()) {
        return true;
    } else {
        stop(STOP_THREAD_ONLY);
        return false;
    }
}

void ThreadPool::stop(STOP_TYPE stopType) 
{
    if (STOP_THREAD_ONLY != stopType) 
    {
        {
            ScopedMutex lock(_cond);
            _push = false;
            _cond.broadcastProducer();
        }
    }

    if (STOP_AFTER_QUEUE_EMPYT == stopType) {
        waitQueueEmpty();
    }
    
    stopThread();
    
    if (STOP_THREAD_ONLY != stopType) {
        clearQueue();
    }
}

void ThreadPool::stopThread() 
{
    if (!_run) 
    {
        return;
    }
    {
        ScopedMutex lock(_cond);
        _run = false;
        _cond.broadcastConsumer();
    }
    _threads.clear();
}

void ThreadPool::waitQueueEmpty() 
{
    while (true)
    {
        if ((0 == getTaskCount()) && (0 == getRestartTaskCount())) {
            break;
        }
        usleep(10000);
    }
}

bool ThreadPool::createThreads() 
{
    for(uint32_t i=0; i < _threadNum; ++i)
	{
            pthread_t pid = createThread(i, 0);
            if (pid <= 0)
            {
                //("Create WorkerLoop with No.%u thread fail!",i);
                return false;
            }
            _threads.push_back(pid);
	}
    return true;
}

pthread_t ThreadPool::createThread(uint32_t idx, uint32_t stack_size) {
    pthread_t pid;
    pthread_attr_t *pAttr = NULL;
    int ret = 0;
    bool success = (stack_size == 0);
    if (stack_size > 0) {
	pAttr = (pthread_attr_t *) malloc(sizeof(pthread_attr_t));
	do {
	    if (pAttr == NULL) {break;}
	    if (pthread_attr_init(pAttr) != 0) {
		::free(pAttr);
		pAttr = NULL;
		break;
	    }
	    if (pthread_attr_setstacksize(pAttr, stack_size) != 0) {break;}
	    if (pthread_attr_setdetachstate(pAttr, PTHREAD_CREATE_JOINABLE) != 0) {break;}
	    // set attr success
	    success = true;
	} while(0);
    }
    if (success) {
	ret = pthread_create(&pid, pAttr, &ThreadPool::workerLoop, this);
	success = (ret == 0);
    }
    if (!success) {
	pid = 0;
    }
    if (pAttr) {
	pthread_attr_destroy(pAttr);
	::free(pAttr);
    }
    return pid;
}

void ThreadPool::clearQueue() 
{
    ScopedMutex lock(_cond);
    while(!_queue.empty()) 
    {
        _queue.pop_front();
    }
    _cond.broadcastProducer();
}

void *ThreadPool::workerLoop(void *args)
{
    ThreadPool *pool = (ThreadPool *)args;
    while(pool->_run) 
    {
        std::function<void()> func;
        bool from_queue = true;
        {
            ScopedMutex lock(pool->_cond);
            while(pool->_run && pool->_restart_queue.empty() &&  pool->_queue.empty()) 
            {
                pool->_cond.consumerWait();
            }
            if (!pool->_run) {
                return NULL;
            }
            if (!pool->_restart_queue.empty()) {
                func = pool->_restart_queue.front();
                pool->_restart_queue.pop_front();
                from_queue = false;
            } else {
                func = pool->_queue.front();
                pool->_queue.pop_front();
            }
            pool->_cond.signalProducer();
        }
        func();
    }
    return NULL;
}
END_NAMESPACE
