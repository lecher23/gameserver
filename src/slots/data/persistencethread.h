#ifndef PERSISTENCETHREAD_H
#define PERSISTENCETHREAD_H

#include <vector>
#include "basicdata.h"

BEGIN_NAMESPACE(slots)
#define MAX_GAME_RECORDS 10240
struct GameRecords{
    std::array<GameRecord, MAX_GAME_RECORDS>  records;
    std::mutex lock;
    size_t next;
    void incr() {
	next ++;
	if (next == MAX_GAME_RECORDS) {
	    CLOG(WARNING) << "Game record full.";
	    next =  MAX_GAME_RECORDS - 1;
	}
    }
    void clear() {
	next = 0;
    }
};


class PersistenceThread{
 public:
    PersistenceThread();
    ~PersistenceThread();

    bool init();
    void stop();
    GameRecord &getUnusedGameRecord();
    void persistent();

    void addData(PersistenceBasePtr data) {
	_data.push_back(data);
    }

 private:
    static void * run(void *args) {
	// connect to db
	PersistenceThread *instance = (PersistenceThread *)args;
	instance->_isRunnig = true;
	while(!instance->_stop){
	    sleep(instance->_exeInterval);
	    for (auto &itr: instance->_data) {
		itr->save2MySQL();
	    }
	    instance->persistent();
	    ++instance->_count;
	}
	instance->_isRunnig = false;
	return NULL;  
    }

    int32_t _exeInterval;
    uint64_t _count;
    bool _stop;
    bool _isRunnig;

    GameRecords  _gameRecords;
    std::vector<PersistenceBasePtr> _data;
    pthread_t tid;
};
END_NAMESPACE
#endif
