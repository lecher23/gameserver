#include "persistencethread.h"
BEGIN_NAMESPACE(slots)
PersistenceThread::PersistenceThread(){
}
    
PersistenceThread::~PersistenceThread(){
}

bool PersistenceThread::init() {
    _exeInterval = 60;
    _stop = false;
}

void PersistenceThread::stop() {
    _stop = true;
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
