#include "gameservice.h"

namespace slots{
    GameService::GameService(){
    }
    
    GameService::~GameService(){
    }

    bool GameService::doJob(CPacket &packet, CResponse &resp) {
	std::string gType;
	GET_PARAM("type", gType, true);
	SBuf bf;
	ResultFormatter rf;
	
	bool ret = false;

	if (gType == "1"){
	    ret = doMultiple(packet);
	    rf.formatSimpleResult(bf, ret, "");
	}
	resp.setBody(bf.GetString());
	return ret;
    }

    bool GameService::doSlots1(CPacket &packet) {

    }

    bool GameService::doMultiple(CPacket &packet){
	std::string incr;
	std::string multiple;
	std::string uid;
	GET_PARAM("incr", incr, true);
	GET_PARAM("uid", uid, true);
	GET_PARAM("multiple", multiple, true);
	uint64_t base;
	if (!cgserver::StringUtil::StrToUInt64(incr.c_str(), base)) {
	    CLOG(WARNING) << "Invalid incr value: " << incr;
	    return false;
	}
	uint32_t dV;
	if (!cgserver::StringUtil::StrToUInt32(multiple.c_str(), dV)) {
	    CLOG(WARNING) << "Invalid multiple value: " << multiple;
	    return false;
	}

	SlotsUserPtr sup;
	if (!SlotsDataCenter::instance().get(uid, sup)) {
	    return false;
	}

	switch (dV) {
	case 0:{
	    // reduce incr
	    UserResource &ur = sup->uRes;
	    ur.fortune -= base;
	    if (ur.fortune < 0) {
		ur.fortune = 0;
	    }
	    ur.changed = true;
	    CLOG(INFO) << "User[" << uid << "] new fortune:" << ur.fortune;
	    break;
	}
	case 2:
	case 4:{
	    UserResource &ur = sup->uRes;
	    ur.fortune += (base <<  (dV/2));
	    ur.changed = true;
	    CLOG(INFO) << "User[" << uid << "] new fortune:" << ur.fortune;
	    break;
	}
	default:{
	    CLOG(WARNING) << "Invalid multiple value: " << dV;
	    return false;
	}
	}
	return true;
    }
}
