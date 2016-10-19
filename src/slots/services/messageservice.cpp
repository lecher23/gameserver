#include "messageservice.h"
#include <util/timeutil.h>
#include <slots/config/slotsconfig.h>
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

MessageService::MessageService(): _dataCenter(SlotsDataCenter::instance()){
}

MessageService::~MessageService(){
}

#define GET_SLOT_USER(uid, user)                                        \
    SlotsUserPtr user;                                                     \
    if (!_dataCenter.slotsUserData->getByUid(uid, user)) { \
        return false;                                                   \
    }

#define MSG_RECV_DAILY_REWARD 1
#define MSG_ENTER_ROOM 2
#define MSG_FINISH_TINY_GAME 5
#define MSG_QUERY_ROOMS_INFO 6

bool MessageService::doJob(CPacket &packet, CResponse &resp) {

    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sType, gType);
    SBuf bf;
    ResultFormatter rf(bf);
    bool ret = false;
    switch(gType){
    case MSG_RECV_DAILY_REWARD:{
        int64_t newFortune;
        ret = getLoginReward(packet, newFortune);
        if (ret) {
            rf.formatSimpleResultWithFortune(newFortune);
        }
        break;
    }
    case MSG_ENTER_ROOM: {
        ret = enterRoom(packet, rf);
        break;
    }
    case MSG_FINISH_TINY_GAME: {
        ret = finishTinyGame(packet, rf);
        // format result
        break;
    }
    case MSG_QUERY_ROOMS_INFO: {
        ret = getRoomInfoInList(packet, rf);
        break;
    }
    default:
        break;
    }
    if (!ret) {
        rf.formatSimpleResult(false, "");
    }
    resp.setBody(bf.GetString());
    return ret;
}


bool MessageService::enterRoom(CPacket &packet, ResultFormatter &rf) {
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sHallID, hallID);
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sRoomID, roomID);
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sUserID, userID);
    auto &hall = _dataCenter.hallOperator;
    RoomInfo room;
    room.userID = userID;
    if (!hall.useRoom(hallID, userID, room))
    {
        CLOG(WARNING) << "User:"<< userID << " enter room " <<
            roomID << " in "<< hallID << "failed.";
        // failed situation sholud not format result in here
        return false;
    }
    // format result
    return true;
}

bool MessageService::getLoginReward(CPacket &packet, int64_t &newFortune) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    GET_SLOT_USER(uid, user);
    LoginReward loginReward;
    if (!SlotsDataCenter::instance().slotsUserData->getDailyReward(uid, loginReward)) {
        return false;
    }
    if (loginReward.recved) {
        newFortune = user->uRes.fortune;
        return true;
    }
    int64_t total = loginReward.runnerReward +
        loginReward.dayReward + loginReward.vipExtra;
    user->uRes.incrFortune(total);
    loginReward.recved = true;
    newFortune = user->uRes.fortune;
    SlotsDataCenter::instance().slotsUserData->updateDailyReward(uid, true);
    return true;
}

bool MessageService::getCargoStatus(CPacket &packet) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    std::string cargoId;
    GET_PARAM("cargo", cargoId, true);
    // TODO
    // get cargo from cache
    // if no record, async request server
    // if has record, return value
    return false;
}

bool MessageService::finishTinyGame(CPacket &packet, ResultFormatter &rf) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    GET_SLOT_USER(uid, user);
    auto &gameStatus = user->gSt;
    user->uRes.incrFortune(gameStatus.tinyGameEarned());
    rf.formatSimpleResultWithFortune(user->uRes.fortune);
    return true;
}

bool MessageService::getRoomInfoInList(CPacket &packet, ResultFormatter &rf) {
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sHallID, hallID);
    std::string roomListStr;
    GET_PARAM(slotconstants::sRoomList, roomListStr, true);
    std::vector<int32_t> roomList;
    if (!cgserver::StringUtil::StrToIntVector(roomListStr, roomList, ',')) {
        return false;
    }

    std::vector<RoomInfo> rooms(roomList.size());
    auto &hall = SlotsDataCenter::instance().hallOperator;
    auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
    auto reserveTime =
        SlotsConfig::getInstance().themeConfig.tsConfig.getRoomReserveTime();
    int32_t curRoomIdx = 0;
    for(auto &roomID: roomList) {
        rooms[curRoomIdx].roomID = roomID;
        if (!hall.getRoomInfo(hallID, rooms[curRoomIdx])) {
            CLOG(WARNING) << "Get room list failed. room id: " << roomID;
            return false;
        }
        rooms[curRoomIdx].update(now, reserveTime);
        ++curRoomIdx;
    }
    rf.formatRoomList(rooms);
    return true;
}

#undef GET_SLOT_USER

END_NAMESPACE
