#include "slotsuserdata.h"

BEGIN_NAMESPACE(slots)

SlotsUserData::SlotsUserData(){
}

SlotsUserData::~SlotsUserData(){
}

bool SlotsUserData::needSave(uint64_t factor) {
    return true;
}

bool SlotsUserData::get(const std::string &uid, SlotsUserPtr &out){
    auto ret = _data.find(uid);
    if (ret == _data.end()) {
	CLOG(INFO) << "Get user["<< uid <<"] info from db.";
	SlotsDB &db = SlotsDB::getInstance();
	out.reset(new SlotsUser);
	if (db.getUserInfoByUserId(uid, *out)) {
	    out->uInfo.changed = false;
	    out->uRes.changed = false;
	    out->uHis.changed = false;
	    out->gDetail.changed = false;
	    set(uid,out);
	} else {
	    CLOG(WARNING) << "Get user[" << uid << "] info failed.";
	    return false;
	}
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
    CLOG(INFO) << "dump data to db.";
    SlotsDB &db = SlotsDB::getInstance();
    for (auto itr = _data.begin(); itr != _data.end(); ++itr) {
	db.update(itr->second);
    }
    CLOG(INFO) << "dump data to db finish.";	    
}

END_NAMESPACE
