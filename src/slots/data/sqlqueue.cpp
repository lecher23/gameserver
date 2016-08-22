#include "sqlqueue.h"
#include <slots/sql/batchsql.h>

BEGIN_NAMESPACE(slots)

SqlQueue::SqlQueue(){
}

SqlQueue::~SqlQueue(){
}

void SqlQueue::save2MySQL(uint64_t factor) {
    if (factor & uint64_t(1)) return;
    _lock.lock();    
    std::vector<std::string> tmpsql(_sqls.size());
    tmpsql.clear();    
    while(!_sqls.empty()) {
	tmpsql.push_back(_sqls.front());
	_sqls.pop();
    }
    _lock.unlock();
    //CLOG(INFO) << "Dump sql, queue size:" << tmpsql.size();
    BatchSql bs(tmpsql);
    auto &pool = cgserver::MysqlConnPool::getInstance();
    pool.doMysqlOperation((cgserver::MysqlOperationBase *) &bs);
}

void SqlQueue::addSql(const std::string &sql) {
    MUTEX_GUARD(_lock);
    _sqls.push(sql);
}

END_NAMESPACE
