#include "slotsuserdata.h"

#include <slots/sql/slotusersaver.h>
#include <util/stringutil.h>
#include <util/timeutil.h>
#include "slots/model/commonsql.h"

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


SlotsUserData::SlotsUserData(cgserver::ProducerConsumerQueue<std::string> &queue)
    :_redisClient(cgserver::RedisClientFactory::getClient()),
     _backupQueue(queue)
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
    out.push_back(SlotCacheStr::sFreeGameCount + hallID);              \
    out.push_back(SlotCacheStr::sTinyGameCount + hallID);              \
    out.push_back(SlotCacheStr::sLink + hallID);

#define GET_INT_VAL_FROM_VEC(sec, attr, bit, index)                     \
    if (result[index].empty()) {                                        \
        CLOG(WARNING) << "data at index:" << index << " not set";       \
        return false;                                                   \
    }                                                                   \
    user.sec.attr = StringUtil::StrToInt##bit##WithDefault(result[index].data(), 0);

#define GET_INT_VAL_FROM_VEC_NO_CHECK(sec, attr, bit, index)            \
    user.sec.attr = StringUtil::StrToInt##bit##WithDefault(result[index].data(), 0);

#define PUSH_INT_VAL_TO_VEC(sec, attr)                         \
    result.push_back(StringUtil::toString(user.sec.attr));

#define GET_USER_RESOURCE_FROM_VEC(start)                               \
    GET_INT_VAL_FROM_VEC(uRes, level, 32, start);                       \
    GET_INT_VAL_FROM_VEC(uRes, exp, 64, start + 1);                     \
    GET_INT_VAL_FROM_VEC(uRes, fortune, 64, start + 2);                 \
    GET_INT_VAL_FROM_VEC(uRes, vipLevel, 32, start + 3);                \
    GET_INT_VAL_FROM_VEC(uRes, vipPoint, 32, start + 4);

#define GET_GAME_HISTORY_FROM_VEC(start)                        \
    GET_INT_VAL_FROM_VEC(gHis, maxFortune, 64, start);          \
    GET_INT_VAL_FROM_VEC(gHis, maxEarned, 64, start + 1);       \
    GET_INT_VAL_FROM_VEC(gHis, totalEarned, 64, start + 2);     \
    GET_INT_VAL_FROM_VEC(gHis, totalBet, 64, start + 3);        \
    GET_INT_VAL_FROM_VEC(gHis, lastLogin, 64, start + 4);       \
    GET_INT_VAL_FROM_VEC(gHis, loginDays, 32, start + 5);       \
    GET_INT_VAL_FROM_VEC(gHis, jackpot, 32, start + 6);

#define GET_PRE_GAME_INFO_FROM_VEC(start)                               \
    GET_INT_VAL_FROM_VEC_NO_CHECK(preGameInfo, freeGameTimes, 32, start);        \
    GET_INT_VAL_FROM_VEC_NO_CHECK(preGameInfo, tinyGameEarned, 64, start + 1);   \
    GET_INT_VAL_FROM_VEC_NO_CHECK(preGameInfo, lastBet, 64, start + 2);          \
    GET_INT_VAL_FROM_VEC_NO_CHECK(preGameInfo, lastLines, 32, start + 3);        \
    GET_INT_VAL_FROM_VEC_NO_CHECK(preGameInfo, lastHallID, 32, start + 4);

#define GET_THEME_HISTORY_FROM_VEC(start)                       \
    GET_INT_VAL_FROM_VEC_NO_CHECK(tHis, bigWinCount, 32, start);         \
    GET_INT_VAL_FROM_VEC_NO_CHECK(tHis, megaWinCount, 32, start + 1);    \
    GET_INT_VAL_FROM_VEC_NO_CHECK(tHis, superWinCount, 32, start + 2);   \
    GET_INT_VAL_FROM_VEC_NO_CHECK(tHis, spinCount, 32, start + 3);       \
    GET_INT_VAL_FROM_VEC_NO_CHECK(tHis, freeGameCount, 32, start + 4);   \
    GET_INT_VAL_FROM_VEC_NO_CHECK(tHis, tinyGameCount, 32, start + 5);   \
    GET_INT_VAL_FROM_VEC_NO_CHECK(tHis, maxLinkCount, 32, start + 6);

#define SET_USER_RESOURCE_TO_VEC()                                      \
    result.push_back(StringUtil::toString(user.uRes.level));            \
    result.push_back(StringUtil::toString(user.uRes.exp));              \
    result.push_back(StringUtil::toString(user.uRes.fortune));          \
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

#define SET_THEME_HISTORY_TO_VEC()                      \
    PUSH_INT_VAL_TO_VEC(tHis, bigWinCount.val);         \
    PUSH_INT_VAL_TO_VEC(tHis, megaWinCount.val);        \
    PUSH_INT_VAL_TO_VEC(tHis, superWinCount.val);       \
    PUSH_INT_VAL_TO_VEC(tHis, spinCount.val);           \
    PUSH_INT_VAL_TO_VEC(tHis, freeGameCount.val);       \
    PUSH_INT_VAL_TO_VEC(tHis, tinyGameCount.val);       \
    PUSH_INT_VAL_TO_VEC(tHis, maxLinkCount.val);


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
    _backupQueue.produce(user.uRes.updateAll(user.uid));
    _backupQueue.produce(user.gHis.updateAll(user.uid));
    auto hId = cgserver::StringUtil::toString(user.hallID);
    for (int32_t i = ThemeHistoryItem::tagBegin();
         i < ThemeHistoryItem::tagEnd(); ++i)
    {
        auto tmp = user.tHis.updateOne(user.uid, hId, i);
        if (!tmp.empty()) {
            _backupQueue.produce(tmp);
        }
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

bool SlotsUserData::setUserToCache(GameContext &user) {
    std::vector<std::string> fields(
        RedisLoginInfoKeys.begin(), RedisLoginInfoKeys.end());
    std::vector<std::string> result;
    SET_USER_RESOURCE_TO_VEC();
    SET_GAME_HISTORY_TO_VEC();
    result.push_back(user.uInfo.fname);
    result.push_back(user.uInfo.country);
    result.push_back(user.uInfo.avatar);
    result.push_back(user.uInfo.male);
    auto &t = user.allTHis;
    for (int i = t.getThemeStart(); i < t.getThemeCount(); ++i) {
        std::string sI = StringUtil::toString(i);

        int32_t tmp = t.getTagValue(i, BIG_WIN_TAG);
        fields.push_back(SlotCacheStr::sBigWin + sI);
        result.push_back(StringUtil::toString(tmp));

        tmp = t.getTagValue(i, MEGA_WIN_TAG);
        fields.push_back(SlotCacheStr::sMegaWin + sI);
        result.push_back(StringUtil::toString(tmp));

        tmp = t.getTagValue(i, SUPER_WIN_TAG);
        fields.push_back(SlotCacheStr::sSuperWin + sI);
        result.push_back(StringUtil::toString(tmp));

        tmp = t.getTagValue(i, NORMAL_GAME_TAG);
        fields.push_back(SlotCacheStr::sGameCount + sI);
        result.push_back(StringUtil::toString(tmp));

        tmp = t.getTagValue(i, FREE_GAME_TAG);
        fields.push_back(SlotCacheStr::sFreeGameCount + sI);
        result.push_back(StringUtil::toString(tmp));

        tmp = t.getTagValue(i, TINY_GAME_TAG);
        fields.push_back(SlotCacheStr::sTinyGameCount + sI);
        result.push_back(StringUtil::toString(tmp));

        tmp = t.getTagValue(i, SIX_LINK_TAG);
        fields.push_back(SlotCacheStr::sLink + sI);
        result.push_back(StringUtil::toString(tmp));
    }
    for (auto &cj: user.oldCj){
        fields.push_back(SlotCacheStr::sCjPrefix
                         + cgserver::StringUtil::toString(cj.aid));
        result.push_back(cj.isRecvReward?
                         SlotCacheStr::sLRecvTrue: SlotCacheStr::sLRecvFalse);
    }
    return _redisClient.Hmset(user.uid, fields, result) == RC_SUCCESS;
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
    if (db.initNewUser(mid, out)) {
        setUidWithMid(mid, out.uid);
        setUserToCache(out);
        return true;
    }
    return false;
}

// just for user login
bool SlotsUserData::getUserByUid(GameContext &out){
    if (!getContextForLogin(out)) {
	SlotsDB &db = SlotsDB::getInstance();
	bool ret = db.getUserData(out);
	if (!ret) {
	    CLOG(WARNING) << "Get user[" << out.uid << "] info failed.";
	    return false;
	}
        setUserToCache(out);
	return true;
    }
    return true;
}

int64_t SlotsUserData::getTinyGameReward(const std::string &uid) {
    std::string result;
    if (_redisClient.Hget(uid, SlotCacheStr::sTinyGameEarned, &result)) {
        return 0;
    }
    return StringUtil::StrToInt64WithDefault(result.data(), 0);
}

bool SlotsUserData::isAchievementExist(const std::string &uid, const std::string &cjID) {
    std::string st;
    if (_redisClient.Hget(uid, SlotCacheStr::sCjPrefix + cjID, &st) != RC_SUCCESS) {
        return false;
    }
    return !st.empty();
}

bool SlotsUserData::checkAchievement(const std::string &uid, const std::string &cjID) {
    std::string st;
    if (_redisClient.Hget(uid, SlotCacheStr::sCjPrefix + cjID, &st) != RC_SUCCESS) {
        return false;
    }
    return st == SlotCacheStr::sLRecvTrue;
}

bool SlotsUserData::setAchievement(
    const std::string &uid, const std::string &cjID, bool recved)
{
    if (_redisClient.Hset(uid, SlotCacheStr::sCjPrefix + cjID,
                          recved? SlotCacheStr::sLRecvTrue: SlotCacheStr::sLRecvFalse)
        != RC_SUCCESS)
    {
        CLOG(WARNING) << "User:" << uid << " achievement :" << cjID << " operate failed.";
        return false;
    }
    _backupQueue.produce(CommonSQL::updateUserAchievement(uid, cjID, recved));
    return true;
}

void SlotsUserData::save2MySQL(uint64_t factor){
    ;// do noting;
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

bool SlotsUserData::addUserFortuneInCache(
    const std::string &uid, int64_t incr, int64_t &res)
{
    if (_redisClient.Hincrby(uid, SlotCacheStr::sMoney, incr, &res) != RC_SUCCESS) {
        CLOG(WARNING) << "Incr user:" << uid << " fortune:" << incr << " failed.";
        return false;
    }
    // save to db
    _backupQueue.produce(CommonSQL::updateUserFortune(uid, res));
    return true;
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
