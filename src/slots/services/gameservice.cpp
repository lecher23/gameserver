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
    }
    resp.setBody(bf.GetString());
    return ret;
}

bool GameService::doTemplePrincess(CPacket &packet, ResultFormatter &rf)
{
    const static ThemeTypes style = ESS_TEMPLE_PRINCESS;
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
        gc.uid = uid;
        gc.hallID = hallID;
        gc.room.roomID = roomID;

        auto &uData = *SlotsDataCenter::instance().slotsUserData;

        if (!uData.getContextForGame(gc)) {
            CLOG(WARNING) << "get user:" << gc.uid << " game context failed.";
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
        if (!uData.setContextForGame(gc)) {
            CLOG(WARNING) << "set user:" << gc.uid << " game context to game failed.";
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
#undef GET_SLOTS_USER_WITH_RETURN
END_NAMESPACE
