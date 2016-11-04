#ifndef BACKUPCONSUMER_H
#define BACKUPCONSUMER_H

#include <thread>
#include "slots/sql/sqlexecutor.h"
#include "util/producerconsumerqueue.h"
#include "mysql/mysqloperationbase.h"
#include "mysql/mysqlconnpool.h"

BEGIN_NAMESPACE(slots)

class BackupConsumer{
public:
    BackupConsumer(cgserver::ProducerConsumerQueue<std::string> &queue)
        :_queue(queue), _pool(cgserver::MysqlConnPool::getInstance()) {
        _pExecutor = (cgserver::MysqlOperationBase *)&_executor;
    }

    ~BackupConsumer() {
        _pExecutor = nullptr;
    }

    bool start() {
        if (!_stopped) {
            CLOG(WARNING) << "already running.";
            return false;
        }
        _t.reset(new std::thread(std::bind(&BackupConsumer::backupLoop, this)));
        _stopped = false;
        return true;
    }

    void stop() {
        _stopped = true;
        int count = 0;
        while(_looping) {
            CLOG(INFO) << "Current backup queue size is:"<< _queue.size();
            _queue.produce(";");
            sleep(3);
            if (++count > 10) break;
        }
        if (_looping) _t->detach();
        else _t->join();
    }

    void backupLoop() {
        // we must stop after _queue is empty
        // bug: at last time, no product, then this thread will wait forever
        _looping = true;
        while(!_stopped or !_queue.empty()) {
            const auto &sql = _queue.consume();
            if (!_pool.doMysqlOperation(_pExecutor)) {
                CLOG(WARNING) << "backup sql:" << sql << " exe failed.";
            }
        }
        _looping = false;
    }

private:
    cgserver::ProducerConsumerQueue<std::string> &_queue;
    cgserver::MysqlConnPool &_pool;

    SQLExecutor _executor;
    cgserver::MysqlOperationBase *_pExecutor{nullptr};
    std::shared_ptr<std::thread> _t;

    bool _stopped{true};
    bool _looping{false};
};

END_NAMESPACE
#endif
