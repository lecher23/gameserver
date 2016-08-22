#ifndef SQLQUEUE_H
#define SQLQUEUE_H

#include <slots/sql/slotsdb.h>
#include <queue>

BEGIN_NAMESPACE(slots)

class SqlQueue: public PersistenceBase{
 public:
    SqlQueue();
    virtual ~SqlQueue();
    virtual void save2MySQL(uint64_t factor);

    void addSql(const std::string &sql);
 private:
    std::queue<std::string> _sqls;
    std::mutex _lock;
};

DF_SHARED_PTR(SqlQueue);

END_NAMESPACE
#endif
