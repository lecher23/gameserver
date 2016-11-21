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
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);
    std::string sequence;
    GET_PARAM(slotconstants::sSequence, sequence, true);
    if (hasBeenProcessed(uid, sequence, resp)) {
        return true;
    }
    SBuf bf;
    ResultFormatter rf(bf);
    bool ret = false;
    switch(gType){
    case MSG_RECV_DAILY_REWARD:{
        int64_t newFortune;
        ret = getLoginReward(packet, newFortune, uid);
        if (ret) {
            rf.formatSimpleResultWithFortune(newFortune);
        }
        break;
    }
    case MSG_FINISH_TINY_GAME: {
        ret = finishTinyGame(packet, rf, uid);
        break;
    }
    case MSG_GET_CJ_REWARD:{
        ret = getAchievementReward(packet, rf, uid);
        break;
    }
    case MSG_RECV_ONLINE_REWARD: {
        ret = recvOnlineReward(packet, rf, uid);
        break;
    }
    }
    if (!ret) {
        rf.formatSimpleResult(false, "");
    }
    setResponse(uid, sequence, bf, resp);
    return true;
}

bool RewardService::getLoginReward(
    CPacket &packet, int64_t &newFortune, const std::string &uid)
{
    LoginReward loginReward;
    if (!_userData.getDailyReward(uid, loginReward) || loginReward.recved) {
        return false;
    }
    int64_t total = loginReward.runnerReward +
        loginReward.dayReward + loginReward.vipExtra;
    if (!_userData.addUserFortuneInCache(uid, total, newFortune)) {
        CLOG(WARNING) << "User:" << uid << " recv login reward:"
                      << total << " failed:set redis fortune failed.";
        return false;
    }
    _userData.updateDailyReward(uid, true);
    return true;
}

bool RewardService::finishTinyGame(
    CPacket &packet, ResultFormatter &rf, const std::string &uid)
{
    auto reward = _userData.getTinyGameReward(uid);
    if (reward == 0) {
        return false;
    }
    int64_t fortune;
    if (!_userData.addUserFortuneInCache(uid, reward, fortune)) {
        CLOG(WARNING) << "User:" << uid << " finsh tiny game with reward:"
                      << reward << " failed:set redis fortune failed.";
        return false;
    }
    rf.formatSimpleResultWithFortune(fortune);
    return true;
}

bool RewardService::getAchievementReward(
    CPacket &packet, ResultFormatter &rf, const std::string &uid)
{
    std::string cjID;
    GET_PARAM(slotconstants::sCjID, cjID, true);
    bool cjOK = _userData.checkAchievement(uid, cjID);
    if (cjOK) {
        const auto &cjInfo =
            SlotsConfig::getInstance().cjConfig.getCjInfo(
                cgserver::StringUtil::StrToInt32WithDefault(cjID.data(), 0));
        int64_t fortune = 0;
        if (!_userData.addUserFortuneInCache(uid, cjInfo.rewardValue, fortune)) {
            CLOG(WARNING) << "User:" << uid << " recv cj:" << cjID << " failed:set redis fortune failed.";
            return false;
        }
        _userData.setAchievement(uid, cjID, true);
        rf.formatSimpleResultWithFortune(fortune);
    }
    return cjOK;
}

bool RewardService::recvOnlineReward(
    CPacket &packet, ResultFormatter &rf, const std::string &uid)
{
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
        CLOG(WARNING) << "User:" << uid << " recv online reward:"
                      << oInfo.rewardValue << " failed:set redis fortune failed.";
        return false;
    }
    rf.formatSimpleResultWithFortune(fortune);
    _userData.recvOnlineGift(uid, true);
    return true;
}

END_NAMESPACE
