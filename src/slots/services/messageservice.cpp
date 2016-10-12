#include "messageservice.h"
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

MessageService::MessageService(){
}

MessageService::~MessageService(){
}

#define MSG_RECV_DAILY_REWARD 1
#define MSG_ENTER_ROOM 2
#define MSG_QUERY_ROOM_PRIZE 3
#define MSG_QUERY_HALL_STATUS 4
#define MSG_FINISH_TINY_GAME 5

bool MessageService::doJob(CPacket &packet, CResponse &resp) {
    static const std::string sFortune = "fortune";

    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sType, gType);
    SBuf bf;
    ResultFormatter rf(bf);
    bool ret = false;
    switch(gType){
    case MSG_RECV_DAILY_REWARD:{
        int64_t newFortune;
        ret = getLoginReward(packet, newFortune);
        rf.formatSimpleResult(ret, sFortune, newFortune);
        break;
    }
    case MSG_QUERY_HALL_STATUS:{
        ret = queryAllRoomInHall(packet, rf);
        break;
    }
    default:
        break;
    }
    resp.setBody(bf.GetString());
    return ret;
}


bool MessageService::enterRoom(CPacket &packet, ResultFormatter &rf) {
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sHallID, hallID);
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sRoomID, roomID);
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sUserID, userID);
    auto &hall = SlotsDataCenter::instance().getHall(hallID);
    if (!hall.useRoom(userID, roomID))
    {
        CLOG(WARNING) << "User:"<< userID << " enter room " <<
            roomID << " in "<< hallID << "failed.";
        // failed situation sholud not format result in here
        return false;
    }
    // format result
    return true;
}

bool MessageService::queryRoomPrize(CPacket &packet, ResultFormatter &rf) {
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sHallID, hallID);
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sRoomID, roomID);
    auto &hall = SlotsDataCenter::instance().getHall(hallID);
    auto prize = hall.getRoomPrize(roomID);
    // format result.
    return true;
}

bool MessageService::queryAllRoomInHall(CPacket &packet, ResultFormatter &rf) {
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sHallID, hallID);
    auto &hall = SlotsDataCenter::instance().getHall(hallID);
    const auto &rooms = hall.getRooms();
    // format result;
    rf.formatRoomsInfo(rooms);
    return true;
}

bool MessageService::getLoginReward(CPacket &packet, int64_t &newFortune) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    SlotsUserPtr user;
    if (!SlotsDataCenter::instance().slotsUserData->getByUid(uid, user)) {
        return false;
    }
    auto &loginReward = user->loginReward;
    if (loginReward.recved) {
        newFortune = user->uRes.fortune;
        return true;
    }
    int64_t total = loginReward.runnerReward +
        loginReward.daysReward + loginReward.specialReward;
    user->uRes.incrFortune(total);
    loginReward.recved = true;
    newFortune = user->uRes.fortune;
    return true;
}

bool MessageService::getCargoStatus(CPacket &packet) {
    std::string uid;
    GET_PARAM("uid", uid, true);
    std::string cargoId;
    GET_PARAM("cargo", cargoId, true);
    // TODO
    // get cargo from cache
    // if no record, async request server
    // if has record, return value
    return false;
}

END_NAMESPACE
