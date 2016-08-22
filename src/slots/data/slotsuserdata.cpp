#include "slotsuserdata.h"

BEGIN_NAMESPACE(slots)

SlotsUserData::SlotsUserData(){
}

SlotsUserData::~SlotsUserData(){
}

bool SlotsUserData::needSave(uint64_t factor) {
    return true;
}

bool SlotsUserData::get(const std::string &id, SlotsUserPtr &out, bool isUserId){
    if (!isUserId || _data.find(id) == _data.end()) {
	//CLOG(INFO) << "Get user["<< id <<"] info from db.";
	SlotsDB &db = SlotsDB::getInstance();
	out.reset(new SlotsUser);
	bool ret = isUserId ? db.getUserInfoByUserId(id, *out)
	    : db.getUserInfoByMachineId(id, *out);
	if (!ret) {
	    CLOG(WARNING) << "Get user[" << id << "] info failed.";
	    return false;
	}
	out->uInfo.changed = false;
	out->uRes.changed = false;
	out->uHis.changed = false;
	out->gDetail.changed = false;
	set(out->uInfo.uid, out);
	return true;
    }
    out = _data[id];
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
