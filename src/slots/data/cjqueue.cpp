#include "cjqueue.h"
#include <slots/sql/batchsql.h>

using namespace cgserver;
BEGIN_NAMESPACE(slots)

CjQueue::CjQueue():_curWrite(0), _curRead(1){
}

CjQueue::~CjQueue(){
}

void CjQueue::save2MySQL(uint64_t factor) {
  swapQueue();
  std::vector<std::string> sqls;
  getSqls(sqls);
  BatchSql bs(sqls);
  auto &pool = MysqlConnPool::getInstance();
  if(!pool.doMysqlOperation((MysqlOperationBase *) &bs)) {
    // if there has error, we should get reason from log
    CLOG(WARNING) << "Save achievement to mysql failed.";
  }
}

void CjQueue::addCj(std::vector<UserCJ> &newCJ) {
  MUTEX_GUARD(_lock);
  for (auto &item: newCJ) {
    _queue[_curWrite].push(item);
  }
}

void CjQueue::getSqls(std::vector<std::string> &sqls) {
  auto &q = _queue[_curRead];
  while(!q.empty()) {
    auto &i = q.front();
    // delete first and insert it.
    MysqlSimpleDelete msd;
    msd.setTable(UserCJStr::sTableName);
    msd.setCondition(UserCJStr::sUid, i.uid, false);
    msd.addCondition(UserCJStr::sAid, i.aid, true, false);
    sqls.push_back(msd.getQuery());
    MysqlSimpleInsert msi;
    msi.setTable(UserCJStr::sTableName);
    msi.setField(UserCJStr::sUid);
    msi.addField(UserCJStr::sAid);
    msi.addField(UserCJStr::sRecv);
    msi.addField(UserCJStr::sProgress);
    msi.addField(UserCJStr::sIsGain);
    msi.addField(UserCJStr::sTime);
    msi.setValue(i.uid);
    msi.addValue(i.aid);
    msi.addValue(i.isRecvReward ? sMysqlTrue : sMysqlFalse);
    msi.addValue(StringUtil::toString(i.progress));
    msi.addValue(i.isGain ? sMysqlTrue : sMysqlFalse);
    msi.addValue(StringUtil::toString(i.time));
    sqls.push_back(msi.getQuery());
    q.pop();
  }
}

void CjQueue::swapQueue() {
  MUTEX_GUARD(_lock);
  _curWrite ^= _curRead;
  _curRead ^= _curWrite;
  _curWrite ^= _curRead;
}

END_NAMESPACE
