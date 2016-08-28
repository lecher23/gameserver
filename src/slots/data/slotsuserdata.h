#ifndef SLOTSUSERDATA_H
#define SLOTSUSERDATA_H

#include <slots/sql/slotsdb.h>

BEGIN_NAMESPACE(slots)

class SlotsUserData: public PersistenceBase{
 public:
    SlotsUserData();
    virtual ~SlotsUserData();
    virtual void save2MySQL(uint64_t factor);
    virtual bool needSave(uint64_t factor);

    /* get user info. if user not exist, read it from db*/
    bool get(const std::string &uid, SlotsUserPtr &out, bool isUserId = true);
    void set(const std::string &uid, SlotsUserPtr &in);

private:
    std::map<std::string, SlotsUserPtr> _data;
    std::mutex _lock;
};

DF_SHARED_PTR(SlotsUserData);

END_NAMESPACE
#endif
