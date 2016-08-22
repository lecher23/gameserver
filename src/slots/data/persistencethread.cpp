#include "persistencethread.h"
BEGIN_NAMESPACE(slots)
PersistenceThread::PersistenceThread():_count(0){
}
    
PersistenceThread::~PersistenceThread(){
}

bool PersistenceThread::init() {
    _exeInterval = 10;
    _stop = false;
    int ret = pthread_create(&tid, NULL, run, this);
    if (ret != 0) {
	CLOG(ERROR) << "Create persistence thread failed:"<< ret;
	return false;
    }
    return true;
}

void PersistenceThread::stop() {
    _stop = true;
    while (_isRunnig) {
	sleep(1);
    }
    for (auto &itr: _data) {
	itr->save2MySQL(_count);
    }
}

GameRecord &PersistenceThread::getUnusedGameRecord() {
    MUTEX_GUARD(_gameRecords.lock);
    _gameRecords.incr();
    return _gameRecords.records[_gameRecords.next];
}

void PersistenceThread::persistent() {
    // persistent gamerecord.
    // TODO
    MUTEX_GUARD(_gameRecords.lock);
    _gameRecords.clear();
}

END_NAMESPACE
