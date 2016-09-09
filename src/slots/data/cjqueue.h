#ifndef CJQUEUE_H
#define CJQUEUE_H

#include <slots/sql/slotsdb.h>
#include <queue>

BEGIN_NAMESPACE(slots)

class CjQueue: public PersistenceBase{
public:
    CjQueue();
    virtual ~CjQueue();
    virtual void save2MySQL(uint64_t factor);

    void addCj(std::vector<UserCJ> &newCj);

private:
    void swapQueue();
    void getSqls(std::vector<std::string> &sqls);

    std::queue<UserCJ> _queue[2];
    int _curWrite;
    int _curRead;
    std::mutex _lock;
};

END_NAMESPACE
#endif
