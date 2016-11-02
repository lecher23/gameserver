#include "rewardservice.h"

BEGIN_NAMESPACE(slots)

RewardService::RewardService():_userData(*SlotsDataCenter::instance().slotsUserData){
}

RewardService::~RewardService(){
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
    LoginReward loginReward;
    if (!_userData.getDailyReward(uid, loginReward) || loginReward.recved) {
        return false;
    }
    int64_t total = loginReward.runnerReward +
        loginReward.dayReward + loginReward.vipExtra;
    if (!_userData.addUserFortuneInCache(uid, total, newFortune)) {
        return false;
    }
    _userData.updateDailyReward(uid, true);
    return true;
}

bool RewardService::finishTinyGame(CPacket &packet, ResultFormatter &rf) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    auto reward = _userData.getTinyGameReward(uid);
    if (reward == 0) {
        return false;
    }
    int64_t fortune;
    if (!_userData.addUserFortuneInCache(uid, reward, fortune)) {
        return false;
    }
    rf.formatSimpleResultWithFortune(fortune);
    return true;
}

bool RewardService::getAchievementReward(CPacket &packet, ResultFormatter &rf) {
    std::string uid;
    std::string cjID;
    GET_PARAM(slotconstants::sUserID, uid, true);
    GET_PARAM(slotconstants::sCjID, cjID, true);
    bool cjOK = _userData.checkAchievement(uid, cjID);
    if (cjOK) {
        const auto &cjInfo =
            SlotsConfig::getInstance().cjConfig.getCjInfo(
                cgserver::StringUtil::StrToInt32WithDefault(cjID.data(), 0));
        // TODO
        // add fortune in cache
        // set cj to recved.
        // formate result.
    }
    return cjOK;
}

bool RewardService::recvOnlineReward(CPacket &packet, ResultFormatter &rf) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    OnlineInfo oInfo;
    if (!_userData.getOnlineInfo(uid, oInfo, 0)) {
        CLOG(WARNING) << "Get user reward info from redis failed.";
        return false;
    }
    if (oInfo.recved) {
        CLOG(WARNING) << "User reward has recved.";
        return false;
    }
    int64_t fortune;
    if (!_userData.addUserFortuneInCache(uid, oInfo.rewardValue, fortune)) {
        return false;
    }
    rf.formatSimpleResultWithFortune(fortune);
    _userData.recvOnlineGift(uid, true);
    return true;
}

END_NAMESPACE
