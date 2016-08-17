#ifndef PERSISTENCETHREAD_H
#define PERSISTENCETHREAD_H

#include <vector>
#include "slotstables.h"

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
	PersistenceThread *p_instance = (PersistenceThread *)args;
	while(!p_instance->_stop){
	    sleep(p_instance->_exeInterval);
	    for (auto &itr: p_instance->_data) {
		itr->save2MySQL();
	    }
	    p_instance->persistent();  
	}
	return NULL;  
    }

    int32_t _exeInterval;
    bool _stop;

    GameRecords  _gameRecords;
    std::vector<PersistenceBasePtr> _data;
};
END_NAMESPACE
#endif
