#include "messageservice.h"
#include <util/timeutil.h>
#include <slots/config/slotsconfig.h>
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

MessageService::MessageService(): _dataCenter(SlotsDataCenter::instance()){
}

MessageService::~MessageService(){
}

#define GET_SLOT_USER(uid, user)                                \
    SlotsUserPtr user;                                          \
    if (!_dataCenter.slotsUserData->getByUid(uid, user)) {      \
        return false;                                           \
    }

#define MSG_RECV_DAILY_REWARD 1
#define MSG_ENTER_ROOM 2
#define MSG_FINISH_TINY_GAME 5
#define MSG_QUERY_ROOMS_INFO 6
#define MSG_GET_CJ_REWARD 7
#define MSG_REPORT_ONLINE_TIME 8
#define MSG_RECV_ONLINE_REWARD 9
#define MSG_QUERY_HALL_INFO 10

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
        break;
    }
    case MSG_QUERY_ROOMS_INFO: {
        ret = getRoomInfoInList(packet, rf);
        break;
    }
    case MSG_GET_CJ_REWARD:{
        ret = getAchievementReward(packet, rf);
        break;
    }
    case MSG_QUERY_HALL_INFO:{
        ret = getHallInfoInList(packet,rf);
        break;
    }
    case MSG_REPORT_ONLINE_TIME:{
        ret = reportOnlineTime(packet, rf);
        break;
    }
    case MSG_RECV_ONLINE_REWARD: {
        ret = recvOnlineReward(packet, rf);
        break;
    }
    }
    if (!ret) {
        rf.formatSimpleResult(false, "");
    }
    resp.setBody(bf.GetString());
    return true;
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
    auto &uRes = user->uRes;
    if (loginReward.recved) {
        newFortune = uRes.fortune.val;
        return true;
    }
    int64_t total = loginReward.runnerReward +
        loginReward.dayReward + loginReward.vipExtra;
    uRes.incrFortune(total);
    loginReward.recved = true;
    newFortune = uRes.fortune.val;
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
    rf.formatSimpleResultWithFortune(user->uRes.fortune.val);
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

bool MessageService::getAchievementReward(CPacket &packet, ResultFormatter &rf) {
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sCjID, cjID);
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    GET_SLOT_USER(uid, user);
    for (auto &cj:user->uCj) {
        if (cjID == cj.aid && !cj.isRecvReward) {
            cj.isRecvReward = true;
            const auto &cjInfo = SlotsConfig::getInstance().cjConfig.getCjInfo(cjID);
            user->uRes.incrFortune(cjInfo.rewardValue);
            rf.formatSimpleResultWithFortune(user->uRes.fortune.val);
            return true;
        }
    }
    return false;
}

bool MessageService::getHallInfoInList(CPacket &packet, ResultFormatter &rf) {
    std::string hallListStr;
    GET_PARAM(slotconstants::sHallList, hallListStr, true);
    std::vector<std::string> hallList;
    cgserver::StringUtil::Split(hallListStr, ',', hallList);
    if (hallList.empty()) {
        return false;
    }

    std::vector<std::string> halls(hallList.size());
    auto &hall = SlotsDataCenter::instance().hallOperator;
    int32_t curHallIdx = 0;
    for(auto &hallID: hallList) {
        if (!hall.getHallPrize(hallID, halls[curHallIdx])) {
            CLOG(WARNING) << "Get room list failed. room id: " << hallID;
            return false;
        }
        halls[curHallIdx];
        ++curHallIdx;
    }
    rf.formatHallList(hallList, halls);
    return true;
}

bool MessageService::reportOnlineTime(CPacket &packet, ResultFormatter &rf) {
    GET_INT32_PARAM_IN_PACKET(packet, slotconstants::sOnlineTime, onlineTime);
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    auto &uData = *_dataCenter.slotsUserData;
    auto &onlineConfig = SlotsConfig::getInstance().onlineConfig;

    OnlineInfo oInfo;
    if (!uData.getOnlineInfo(uid, oInfo, onlineConfig.getDefaultReward())) {
        return false;
    }
    auto curLevel = oInfo.rewardLevel;
    if (!oInfo.recved) {
        rf.formatOnlineInfo(oInfo, curLevel, MAX_ONLINE_TIME);
        return true;
    }
    auto curTimeSum = _dataCenter.slotsUserData->incrOnlineTime(uid, onlineTime);

    int32_t extTimeNeed = 0;
    int64_t reward = 0;
    auto nextLevel = onlineConfig.nextLevel(
         curLevel, curTimeSum, extTimeNeed, reward);
    if (nextLevel == curLevel) {
        rf.formatOnlineInfo(oInfo, curLevel, extTimeNeed);
        return true;
    }
    oInfo.recved = (reward == 0);
    oInfo.rewardLevel = nextLevel;
    oInfo.rewardValue = reward;
    if (!uData.setOnlineInfo(uid, oInfo)){
        return false;
    }
    rf.formatOnlineInfo(oInfo, curLevel, extTimeNeed);
    return true;
}

bool MessageService::recvOnlineReward(CPacket &packet, ResultFormatter &rf) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    auto &uData = *_dataCenter.slotsUserData;
    GET_SLOT_USER(uid, user);
    OnlineInfo oInfo;
    if (!uData.getOnlineInfo(uid, oInfo, 0)) {
        CLOG(WARNING) << "Get user reward info from redis failed.";
        return false;
    }
    if (oInfo.recved) {
        CLOG(WARNING) << "User reward has recved.";
        return false;
    }
    auto &uRes = user->uRes;
    uRes.incrFortune(oInfo.rewardValue);
    user->uHis.newFortune(uRes.fortune.val);
    uData.recvOnlineGift(uid, true);
    rf.formatSimpleResultWithFortune(uRes.fortune.val);
    return true;
}

#undef GET_SLOT_USER

END_NAMESPACE
