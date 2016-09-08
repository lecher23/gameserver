#include "gameservice.h"
#include <util/luatoolfactory.h>
#include <slots/processors/allprocessors.h>

BEGIN_NAMESPACE(slots)
GameService::GameService(){
}
    
GameService::~GameService(){
}

bool GameService::doJob(CPacket &packet, CResponse &resp) {
    std::string gType;
    GET_PARAM("type", gType, true);
    SBuf bf;
    ResultFormatter rf(bf);
	
    bool ret = false;
    if (gType == "0"){
	ret = doSlots1(packet, rf);
	if (!ret) {
	    rf.formatSimpleResult(ret, "");
	}
    }else if (gType == "1"){
	ret = doMultiple(packet);
	rf.formatSimpleResult(ret, "");
    }
    resp.setBody(bf.GetString());
    return ret;
}

#define GET_SLOTS_USER_WITH_BREAK(uid, dest)                            \
    if (!SlotsDataCenter::instance().slotsUserData->getByUid(uid, dest)) { \
	break;                                                          \
    }


bool GameService::doSlots1(CPacket &packet, ResultFormatter &rf)
{
    const static SlotsStyle style = ESS_SLOTS_1;
    std::string detail;
    bool ret = false;
    do {
	std::string uid;
	std::string bet;
	GET_PARAM_WITH_BREAK("uid", uid, true);
	GET_PARAM_WITH_BREAK("bet", bet, true);
	
	int64_t betVal;
	if (!cgserver::StringUtil::StrToInt64(bet.c_str(), betVal) || betVal < 0) {
	    CLOG(WARNING) << "Invalid bet value: " << bet;
	    break;
	}
	GameContext gc;
	gc.gameInfo.bet = betVal;
	gc.gameInfo.gType = style;
	GET_SLOTS_USER_WITH_BREAK(uid, gc.user);
	GameProcessor gProcessor;
	if (!gProcessor.process(gc)) {
	    break;
	}
	HistoryProcessor hProcessor;
	if (!hProcessor.process(gc)) {
	    break;
	}
	AchievementProcessor aProcessor;
	if (!aProcessor.process(gc)) {
	    break;
	}
	//SlotsDataCenter::instance().slotsEvent.playGame(user, data);
	ret = true;
	rf.formatGameResult(gc.user->uRes, gc.gameInfo.earned, gc.gameInfo.detail);
    } while (false);
    if (!ret) {
	rf.formatSimpleResult(ret, "");
    }
    return ret;
}

bool GameService::doMultiple(CPacket &packet){
    std::string incr;
    std::string multiple;
    std::string uid;
    GET_PARAM("incr", incr, true);
    GET_PARAM("uid", uid, true);
    GET_PARAM("multiple", multiple, true);
    int64_t incrVal;
    if (!cgserver::StringUtil::StrToInt64(incr.c_str(), incrVal) ||
	incrVal < 0) {
	CLOG(WARNING) << "Invalid incr value: " << incr;
	return false;
    }
    uint32_t dV;
    if (!cgserver::StringUtil::StrToUInt32(multiple.c_str(), dV)) {
	CLOG(WARNING) << "Invalid multiple value: " << multiple;
	return false;
    }

    SlotsUserPtr sup;
    if (!SlotsDataCenter::instance().slotsUserData->getByUid(uid, sup)) {
	return false;
    }

    switch (dV) {
    case 0:{
	// reduce incr
	UserResource &ur = sup->uRes;
	ur.incrFortune(-incrVal);
	CLOG(INFO) << "User[" << uid << "] new fortune:" << ur.fortune;
	break;
    }
    case 2:
    case 4:{
	UserResource &ur = sup->uRes;
	ur.incrFortune(incrVal * (dV -1));
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

#undef GET_SLOTS_USER_WITH_RETURN
END_NAMESPACE
