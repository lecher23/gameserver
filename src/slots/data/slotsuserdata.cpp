#include "slotsuserdata.h"

BEGIN_NAMESPACE(slots)

SlotsUserData::SlotsUserData(){
}

SlotsUserData::~SlotsUserData(){
}

bool SlotsUserData::needSave(uint64_t factor) {
    return true;
}

bool SlotsUserData::getByMid(const std::string &mid, SlotsUserPtr &out) {
    std::string uid;
    SlotsDB &db = SlotsDB::getInstance();
    // this user exist
    if(db.getUserIdByMachineId(mid, uid)) {
        std::cout << "User mid:" << mid << " exist.\n";
        return getByUid(uid, out);
    }
    std::cout << "User mid:" << mid << " not exist.\n";
    if (out.get() == nullptr) {
        out.reset(new SlotsUser);
    }
    return db.getUserInfoByMachineId(mid, *out);
}

bool SlotsUserData::getByUid(const std::string &uid, SlotsUserPtr &out){
    if (_data.find(uid) == _data.end()) {
	SlotsDB &db = SlotsDB::getInstance();
	out.reset(new SlotsUser);
	bool ret = db.getUserInfoByUserId(uid, *out);
	if (!ret) {
	    CLOG(WARNING) << "Get user[" << uid << "] info failed.";
	    return false;
	}
	out->uInfo.changed = false;
	out->uRes.changed = false;
	out->uHis.changed = false;
	out->gDetail.changed = false;
	set(out->uInfo.uid, out);
	return true;
    }
    out = _data[uid];
    return true;
}

void SlotsUserData::set(const std::string &uid, SlotsUserPtr &in) {
    MUTEX_GUARD(_lock);
    _data[uid] = in;
}

void SlotsUserData::save2MySQL(uint64_t factor){
    MUTEX_GUARD(_lock);
    //CLOG(INFO) << "dump data to db.";
    SlotsDB &db = SlotsDB::getInstance();
    for (auto itr = _data.begin(); itr != _data.end(); ++itr) {
	db.update(itr->second);
    }
    //CLOG(INFO) << "dump data to db finish.";	    
}

END_NAMESPACE
