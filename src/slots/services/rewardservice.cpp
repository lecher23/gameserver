#include "rewardservice.h"

BEGIN_NAMESPACE(slots)

RewardService::RewardService():_dataCenter(SlotsDataCenter::instance()){
}

RewardService::~RewardService(){
}


#define GET_SLOT_USER(uid, user)                                \
    SlotsUserPtr user;                                          \
    if (!_dataCenter.slotsUserData->getByUid(uid, user)) {      \
        return false;                                           \
    }

#define MSG_RECV_DAILY_REWARD 1
#define MSG_FINISH_TINY_GAME 5
#define MSG_GET_CJ_REWARD 7
#define MSG_RECV_ONLINE_REWARD 9

bool RewardService::doJob(CPacket &packet, CResponse &resp) {
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
    case MSG_FINISH_TINY_GAME: {
        ret = finishTinyGame(packet, rf);
        break;
    }
    case MSG_GET_CJ_REWARD:{
        ret = getAchievementReward(packet, rf);
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

bool RewardService::getLoginReward(CPacket &packet, int64_t &newFortune) {
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

bool RewardService::finishTinyGame(CPacket &packet, ResultFormatter &rf) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    GET_SLOT_USER(uid, user);
    auto &gameStatus = user->gSt;
    user->uRes.incrFortune(gameStatus.tinyGameEarned());
    rf.formatSimpleResultWithFortune(user->uRes.fortune.val);
    return true;
}

bool RewardService::getAchievementReward(CPacket &packet, ResultFormatter &rf) {
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

bool RewardService::recvOnlineReward(CPacket &packet, ResultFormatter &rf) {
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

END_NAMESPACE
