#include "slotsuserdata.h"

#include <slots/sql/slotusersaver.h>
#include <util/stringutil.h>
#include <util/timeutil.h>

BEGIN_NAMESPACE(slots)

using namespace cgserver;

const std::vector<std::string> SlotsUserData::RedisGameInfoKeys = {
    SlotCacheStr::sLevel,// 1
    SlotCacheStr::sExp,// 2
    SlotCacheStr::sMoney,// 3
    SlotCacheStr::sVip,// 4
    SlotCacheStr::sVipPoint,// 5

    SlotCacheStr::sMaxFortune,// 6
    SlotCacheStr::sMaxEarned,// 7
    SlotCacheStr::sTotalEarned,// 8
    SlotCacheStr::sTotalBet,// 9
    SlotCacheStr::sLastLogin,// 10
    SlotCacheStr::sLoginDays,// 11
    SlotCacheStr::sJackpot,// 12

    SlotCacheStr::sFreeGameTimes,// 13
    SlotCacheStr::sTinyGameEarned,// 14
    SlotCacheStr::sLastBet,// 15
    SlotCacheStr::sLastLines,// 16
    SlotCacheStr::sLastHall// 17
};

const std::vector<std::string> SlotsUserData::RedisLoginInfoKeys = {
    SlotCacheStr::sLevel,// 1
    SlotCacheStr::sExp,// 2
    SlotCacheStr::sMoney,// 3
    SlotCacheStr::sVip,// 4
    SlotCacheStr::sVipPoint,// 5

    SlotCacheStr::sMaxFortune,// 6
    SlotCacheStr::sMaxEarned,// 7
    SlotCacheStr::sTotalEarned,// 8
    SlotCacheStr::sTotalBet,// 9
    SlotCacheStr::sLastLogin,// 10
    SlotCacheStr::sLoginDays,// 11
    SlotCacheStr::sJackpot,// 12

    SlotCacheStr::sName,// 13
    SlotCacheStr::sCountry,// 14
    SlotCacheStr::sAvatar,// 15
    SlotCacheStr::sSex// 16
};


SlotsUserData::SlotsUserData()
:_redisClient(cgserver::RedisClientFactory::getClient())
{
}

SlotsUserData::~SlotsUserData(){
}

bool SlotsUserData::needSave(uint64_t factor) {
    return true;
}

#define MAKE_GAME_CONTEXT_FIELDS(out)                                   \
    std::vector<std::string> out(                                       \
        RedisGameInfoKeys.begin(), RedisGameInfoKeys.end());            \
    std::string hallID = cgserver::StringUtil::toString(user.hallID);   \
    out.push_back(SlotCacheStr::sBigWin + hallID);                     \
    out.push_back(SlotCacheStr::sMegaWin + hallID);                    \
    out.push_back(SlotCacheStr::sSuperWin + hallID);                   \
    out.push_back(SlotCacheStr::sGameCount + hallID);                  \
    out.push_back(SlotCacheStr::sFreeGameTimes + hallID);              \
    out.push_back(SlotCacheStr::sTinyGameCount + hallID);              \
    out.push_back(SlotCacheStr::sLink + hallID);

#define GET_INT_VAL_FROM_VEC(sec, attr, bit, index)                     \
    user.sec.attr = StringUtil::StrToInt##bit##WithDefault(result[index].data(), 0);

#define PUSH_INT_VAL_TO_VEC(sec, attr)                         \
    result.push_back(StringUtil::toString(user.sec.attr));

#define GET_USER_RESOURCE_FROM_VEC(start)                               \
    user.uRes.level = StringUtil::StrToInt32WithDefault(result[start].data(), 0); \
    user.uRes.exp = StringUtil::StrToInt64WithDefault(result[start + 1].data(), 0); \
    user.uRes.fortune = StringUtil::StrToInt64WithDefault(result[start + 2].data(), 0); \
    user.uRes.vipLevel = StringUtil::StrToInt32WithDefault(result[start + 3].data(), 0); \
    user.uRes.vipPoint = StringUtil::StrToInt32WithDefault(result[start + 4].data(), 0);

#define GET_GAME_HISTORY_FROM_VEC(start)                                \
    user.gHis.maxFortune = StringUtil::StrToInt64WithDefault(result[start].data(), 0); \
    user.gHis.maxEarned = StringUtil::StrToInt64WithDefault(result[start + 1].data(), 0); \
    user.gHis.totalEarned = StringUtil::StrToInt64WithDefault(result[start + 2].data(), 0); \
    user.gHis.totalBet = StringUtil::StrToInt64WithDefault(result[start + 3].data(), 0); \
    user.gHis.lastLogin = StringUtil::StrToInt64WithDefault(result[start + 4].data(), 0); \
    user.gHis.loginDays = StringUtil::StrToInt32WithDefault(result[start + 5].data(), 0); \
    user.gHis.jackpot = StringUtil::StrToInt32WithDefault(result[start + 5].data(), 0);

#define GET_PRE_GAME_INFO_FROM_VEC(start)                               \
    GET_INT_VAL_FROM_VEC(preGameInfo, freeGameTimes, 32, start);        \
    GET_INT_VAL_FROM_VEC(preGameInfo, tinyGameEarned, 64, start + 1);   \
    GET_INT_VAL_FROM_VEC(preGameInfo, lastBet, 64, start + 2);          \
    GET_INT_VAL_FROM_VEC(preGameInfo, lastLines, 32, start + 3);        \
    GET_INT_VAL_FROM_VEC(preGameInfo, lastHallID, 32, start + 4);

#define GET_THEME_HISTORY_FROM_VEC(start)                       \
    GET_INT_VAL_FROM_VEC(tHis, bigWinCount, 32, start);         \
    GET_INT_VAL_FROM_VEC(tHis, megaWinCount, 32, start + 1);    \
    GET_INT_VAL_FROM_VEC(tHis, superWinCount, 32, start + 2);   \
    GET_INT_VAL_FROM_VEC(tHis, spinCount, 32, start + 3);       \
    GET_INT_VAL_FROM_VEC(tHis, freeGameCount, 32, start + 4);   \
    GET_INT_VAL_FROM_VEC(tHis, tinyGameCount, 32, start + 5);   \
    GET_INT_VAL_FROM_VEC(tHis, maxLinkCount, 32, start + 6);

#define SET_USER_RESOURCE_TO_VEC()                                      \
    result.push_back(StringUtil::toString(user.uRes.level.val));        \
    result.push_back(StringUtil::toString(user.uRes.exp.val));          \
    result.push_back(StringUtil::toString(user.uRes.fortune.val));      \
    result.push_back(StringUtil::toString(user.uRes.vipLevel.val));     \
    result.push_back(StringUtil::toString(user.uRes.vipPoint.val));

#define SET_GAME_HISTORY_TO_VEC()               \
    PUSH_INT_VAL_TO_VEC(gHis, maxFortune);       \
    PUSH_INT_VAL_TO_VEC(gHis, maxEarned);        \
    PUSH_INT_VAL_TO_VEC(gHis, totalEarned);      \
    PUSH_INT_VAL_TO_VEC(gHis, totalBet);         \
    PUSH_INT_VAL_TO_VEC(gHis, lastLogin);        \
    PUSH_INT_VAL_TO_VEC(gHis, loginDays);        \
    PUSH_INT_VAL_TO_VEC(gHis, jackpot);

#define SET_PRE_GAME_INFO_TO_VEC()                      \
    PUSH_INT_VAL_TO_VEC(preGameInfo, freeGameTimes);     \
    PUSH_INT_VAL_TO_VEC(preGameInfo, tinyGameEarned);    \
    PUSH_INT_VAL_TO_VEC(preGameInfo, lastBet);           \
    PUSH_INT_VAL_TO_VEC(preGameInfo, lastLines);         \
    PUSH_INT_VAL_TO_VEC(preGameInfo, lastHallID);

#define SET_THEME_HISTORY_TO_VEC()              \
    PUSH_INT_VAL_TO_VEC(tHis, bigWinCount);      \
    PUSH_INT_VAL_TO_VEC(tHis, megaWinCount);     \
    PUSH_INT_VAL_TO_VEC(tHis, superWinCount);    \
    PUSH_INT_VAL_TO_VEC(tHis, spinCount);        \
    PUSH_INT_VAL_TO_VEC(tHis, freeGameCount);    \
    PUSH_INT_VAL_TO_VEC(tHis, tinyGameCount);    \
    PUSH_INT_VAL_TO_VEC(tHis, maxLinkCount);


bool SlotsUserData::getContextForGame(GameContext &user) {
    MAKE_GAME_CONTEXT_FIELDS(fields);
    std::vector<std::string> result;
    auto ret = _redisClient.Hmget(user.uid, fields, &result);
    if (result.size() != fields.size()) {
        CLOG(WARNING) << "Get game context faile with user id:" << user.uid;
        return false;
    }
    GET_USER_RESOURCE_FROM_VEC(0);
    GET_GAME_HISTORY_FROM_VEC(5);
    GET_PRE_GAME_INFO_FROM_VEC(12);
    GET_THEME_HISTORY_FROM_VEC(17);
    return true;
}

bool SlotsUserData::setContextForGame(GameContext &user) {
    MAKE_GAME_CONTEXT_FIELDS(fields);
    std::vector<std::string> result;
    SET_USER_RESOURCE_TO_VEC();
    SET_GAME_HISTORY_TO_VEC();
    SET_PRE_GAME_INFO_TO_VEC();
    SET_THEME_HISTORY_TO_VEC();
    if (_redisClient.Hmset(user.uid, fields, result) != RC_SUCCESS) {
        CLOG(WARNING) << "Set game context faile with user id:" << user.uid;
        return false;
    }
    return true;
}

bool SlotsUserData::getContextForLogin(GameContext &user) {
    std::vector<std::string> result;
    auto ret = _redisClient.Hmget(user.uid, RedisLoginInfoKeys, &result);
    if (result.size() != RedisLoginInfoKeys.size()) {
        CLOG(WARNING) << "Get login context faile with user id:" << user.uid;
        return false;
    }
    GET_USER_RESOURCE_FROM_VEC(0);
    GET_GAME_HISTORY_FROM_VEC(5);
    user.uInfo.fname = result[12];
    user.uInfo.country = result[13];
    user.uInfo.avatar = result[14];
    user.uInfo.male = result[15];
    return true;
}

bool SlotsUserData::updateLoginInfo(const std::string &uid, int64_t loginTimestamp) {
    if (_redisClient.Hset(uid, SlotCacheStr::sLastLogin,
                          StringUtil::toString(loginTimestamp)) != RC_SUCCESS)
    {
        CLOG(WARNING) << "Set login context faile with user id:" << uid;
        return false;
    }
    return true;
}

#undef GET_INT_VAL_FROM_VEC
#undef MAKE_GAME_CONTEXT_FIELDS

bool SlotsUserData::getUserByMid(const std::string &mid, GameContext &out) {
    //get uid from cache
    if (getUidByMid(mid, out.uid)) {
        return getUserByUid(out);
    }
    // get uid from db
    SlotsDB &db = SlotsDB::getInstance();
    if(db.getUserIdByMachineId(mid, out.uid)) {
        // user exist, save to redis
        setUidWithMid(mid, out.uid);
        return getUserByUid(out);
    }
    /**init new user**/
    // if (db.initNewUser(mid, out)) {
    //     setUidWithMid(mid, out.uid);
    //     setUserToCache(out);
    //     return true;
    // }
    return false;
}

// just for user login
bool SlotsUserData::getUserByUid(GameContext &out){
    if (!getContextForLogin(out)) {
	SlotsDB &db = SlotsDB::getInstance();
        bool ret = false;
	//bool ret = db.getSlotsUserByUid(out);
	if (!ret) {
	    CLOG(WARNING) << "Get user[" << out.uid << "] info failed.";
	    return false;
	}
        // todo: set all info to db.
        setUserToCache(out);
	return true;
    }
    return true;
}

void SlotsUserData::save2MySQL(uint64_t factor){
    MUTEX_GUARD(_lock);
    SlotUserSaver saver;
    auto &pool = cgserver::MysqlConnPool::getInstance();
    for (auto itr = _data.begin(); itr != _data.end(); ++itr) {
        saver.setUser(itr->second);
        if (!pool.doMysqlOperation((cgserver::MysqlOperationBase *) &saver)) {
            CLOG(WARNING) << "save user" << itr->second->uInfo.uid << " info falied.";
        }
    }
}

#define GENERATE_LOGIN_KEY(key, uid) \
    std::string key = SlotCacheStr::sDailyKeyPrefix + uid;

#define GENERATE_MID_KEY(key, mid) \
    std::string key = SlotCacheStr::sMachineIDPrefix + mid;

#define REDIS_EASY_HSET(key, param, value)                      \
    {                                                           \
        int ret = _redisClient.Hset(key, param, value);         \
        if (ret != 0) {                                         \
            CLOG(WARNING) << "HSet login info for key: "         \
                          << key << "failed. code:" << ret;     \
            return;                                             \
        }                                                       \
    }


#define REDIS_EASY_HMSET(key, value)                            \
    {                                                           \
        int ret = _redisClient.Hmset(key, value);               \
        if (ret != 0) {                                         \
            CLOG(WARNING) << "HMSet login info for key: "         \
                          << key << "failed. code:" << ret;     \
            return;                                             \
        }                                                       \
    }

#define REDIS_EASY_EXPIREAT(key, ts)                            \
    {                                                           \
        int ret = _redisClient.Expireat(key, ts);               \
        if (ret != 0) {                                         \
            CLOG(WARNING) << "Set expire time for key:"         \
                          << key << "failed. code:" << ret;     \
            return;                                             \
        }                                                       \
    }

void SlotsUserData::setDailyReward(
    const std::string &userID, const LoginReward &lR)
{
    GENERATE_LOGIN_KEY(key, userID);
    std::map<std::string, std::string> cacheInfo;
    cacheInfo[SlotCacheStr::sLRecvKey] =
        lR.recved ? SlotCacheStr::sLRecvTrue: SlotCacheStr::sLRecvFalse;
    cacheInfo[SlotCacheStr::sLRunnerIdKey] = cgserver::StringUtil::toString(lR.runnerIdx);
    cacheInfo[SlotCacheStr::sLRunnerRewardKey] =
        cgserver::StringUtil::toString(lR.runnerReward);
    cacheInfo[SlotCacheStr::sLVipKey] = cgserver::StringUtil::toString(lR.vipExtra);
    cacheInfo[SlotCacheStr::sLDayKey] = cgserver::StringUtil::toString(lR.dayReward);

    REDIS_EASY_HMSET(key, cacheInfo);
    REDIS_EASY_EXPIREAT(key, cgserver::CTimeUtil::getTomorrowMorning());
}

void SlotsUserData::updateDailyReward(const std::string &userID, bool recved)
{
    GENERATE_LOGIN_KEY(key, userID);
    REDIS_EASY_HSET(key, SlotCacheStr::sLRecvKey,
                    recved ? SlotCacheStr::sLRecvTrue: SlotCacheStr::sLRecvFalse);
}

#define PARSE_CACHE_VAL_TO_INT32(key, dest)                             \
    if (!cgserver::StringUtil::StrToInt32(                              \
            out[key].data(), dest))                                     \
    {                                                                   \
        CLOG(WARNING) << "Invalid " << key << ":" << out[key];          \
        return false;                                                   \
    }


// return key exist
bool SlotsUserData::getDailyReward(
    const std::string &userID, LoginReward &reward)
{
    std::map<std::string, std::string> out;
    GENERATE_LOGIN_KEY(key, userID);
    auto ret = _redisClient.Hgetall(key, &out);
    if (ret != 0) {
        CLOG(INFO) << "Get daily reward info failed, code:" << ret;
        return false;
    }
    reward.recved = (out[SlotCacheStr::sLRecvKey] == SlotCacheStr::sLRecvTrue);
    PARSE_CACHE_VAL_TO_INT32(SlotCacheStr::sLRunnerIdKey, reward.runnerIdx);
    PARSE_CACHE_VAL_TO_INT32(SlotCacheStr::sLRunnerRewardKey, reward.runnerReward);
    PARSE_CACHE_VAL_TO_INT32(SlotCacheStr::sLVipKey, reward.vipExtra);
    PARSE_CACHE_VAL_TO_INT32(SlotCacheStr::sLDayKey, reward.dayReward);
    return true;
}

int32_t SlotsUserData::incrOnlineTime(const std::string &userID, int32_t incrVal) {
    int64_t curVal = 0;
    auto ret = _redisClient.Hincrby(
        SlotCacheStr::sDailyKeyPrefix + userID,
        SlotCacheStr::sOLTime, incrVal, &curVal);
    if (ret != RC_SUCCESS) {
        CLOG(INFO) << "Incr online time for user:" << userID << " failed. IncrVal:"
                   << incrVal <<", code:" << ret;
        curVal = 0;
    }
    return curVal;
}

bool SlotsUserData::getOnlineInfo(
    const std::string &userID, OnlineInfo &onlineInfo, int64_t defaultReward)
{
    static const std::vector<std::string> fields = {SlotCacheStr::sOLRecv,
        SlotCacheStr::sOLLevel, SlotCacheStr::sOLReward};
    std::vector<std::string> result;
    auto ret =
        _redisClient.Hmget(SlotCacheStr::sDailyKeyPrefix + userID, fields, &result);
    if (result.size() != fields.size() ) {
        CLOG(INFO) << "Invalid result for online info with userID:" << userID;
        return false;
    }
    onlineInfo.recved = (result[0] != SlotCacheStr::sLRecvFalse);
    onlineInfo.rewardLevel =
        cgserver::StringUtil::StrToInt32WithDefault(result[1].data(), 0);
    onlineInfo.rewardValue =
        cgserver::StringUtil::StrToInt32WithDefault(result[2].data(), defaultReward);
    return true;
}

bool SlotsUserData::setOnlineInfo(const std::string &userID, OnlineInfo &onlineInfo) {
    GENERATE_LOGIN_KEY(key, userID);
    static const std::vector<std::string> fields = {SlotCacheStr::sOLTime,
        SlotCacheStr::sOLRecv,SlotCacheStr::sOLLevel, SlotCacheStr::sOLReward};
    std::vector<std::string> vals;
    vals.push_back(SlotCacheStr::sLRecvFalse); // 0
    vals.push_back(
        onlineInfo.recved ? SlotCacheStr::sLRecvTrue: SlotCacheStr::sLRecvFalse);
    vals.push_back(cgserver::StringUtil::toString(onlineInfo.rewardLevel));
    vals.push_back(cgserver::StringUtil::toString(onlineInfo.rewardValue));
    if (_redisClient.Hmset(key, fields, vals) != RC_SUCCESS) {
        CLOG(WARNING) << "Set online info for use:" << userID << " failed.";
        return false;
    }
    return true;
}

void SlotsUserData::recvOnlineGift(const std::string &userID, bool recved) {
    GENERATE_LOGIN_KEY(key, userID);
    REDIS_EASY_HSET(key, SlotCacheStr::sOLRecv,
                    recved ? SlotCacheStr::sLRecvTrue: SlotCacheStr::sLRecvFalse);
}

bool SlotsUserData::setUserToCache(GameContext &user) {
    // uid is key
    return false;
}

bool SlotsUserData::addUserFortuneInCache(
    const std::string &uid, int64_t incr, int64_t &res)
{
    return _redisClient.Hincrby(uid, SlotCacheStr::sMoney, incr, &res) == RC_SUCCESS;
}

bool SlotsUserData::getUidByMid(const std::string &mid, std::string &uid) {
    GENERATE_MID_KEY(key, mid);
    if (_redisClient.Get(key, &uid) != RC_SUCCESS) {
        return false;
    }
    return !uid.empty();
}

void SlotsUserData::setUidWithMid(const std::string &mid, std::string &uid) {
    GENERATE_MID_KEY(key, mid);
    if (_redisClient.Set(key, uid) != RC_SUCCESS) {
        CLOG(WARNING) << "save user mid:" << mid << " with uid:" << uid << " failed.";
    }
}

#undef PARSE_CACHE_VAL_TO_INT32
#undef GENERATE_LOGIN_KEY
#undef GENERATE_MID_KEY
#undef REDIS_EASY_HSET
#undef REDIS_EASY_EXPIREAT

END_NAMESPACE
