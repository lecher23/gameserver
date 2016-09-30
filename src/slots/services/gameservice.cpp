#include "gameservice.h"
#include <util/luatoolfactory.h>

BEGIN_NAMESPACE(slots)
GameService::GameService(){
}

GameService::~GameService(){
}

bool GameService::doJob(CPacket &packet, CResponse &resp) {
    std::string gType;
    GET_PARAM(slotconstants::sType, gType, true);
    SBuf bf;
    ResultFormatter rf(bf);

    bool ret = false;
    if (gType == "2"){
	ret = doTemplePrincess(packet, rf);
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


bool GameService::doTemplePrincess(CPacket &packet, ResultFormatter &rf)
{
    const static SlotsStyle style = ESS_TEMPLE_PRINCESS;
    std::string detail;
    bool ret = false;
    do {
	std::string uid;
        int32_t iUid;
        GET_PARAM_WITH_BREAK(slotconstants::sUserID, uid, true);
        if (!cgserver::StringUtil::StrToInt32(uid.c_str(), iUid)) {
            break;
        }
        int64_t bet;
        if (!getIntVal(packet, slotconstants::sTotalBet, bet)) {
            break;
        }
        int32_t lineNumber;
        if (!getIntVal(packet, slotconstants::sLineNumber, lineNumber)) {
            break;
        }
        int32_t roomID;
        if (!getIntVal(packet, slotconstants::sRoomID, roomID)) {
            break;
        }
        int32_t hallID;
        if (!getIntVal(packet, slotconstants::sHallID, hallID)) {
            break;
        }
        CLOG(INFO) << "u:" << uid << ", r:" << roomID << ", b:" << bet;

	GameContext gc;
	gc.gameInfo.bet = bet;
        gc.gameInfo.lineNumber = lineNumber;
        gc.gameInfo.gType = style;
        gc.uid = iUid;
        gc.hallID = hallID;
        gc.roomID = roomID;

        if (!SlotsDataCenter::instance().slotsUserData->getByUid(uid, gc.user)) {
            break;
        }
	if (!_gProcessor.process(gc)) {
            CLOG(WARNING) << "generate game result failed";
	    break;
	}
	if (!_hProcessor.process(gc)) {
            CLOG(WARNING) << "process game result failed";
	    break;
	}
	if (!_aProcessor.process(gc)) {
            CLOG(WARNING) << "process achivement failed";
	    break;
	}
	ret = true;
	rf.formatGameResult(gc);
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
