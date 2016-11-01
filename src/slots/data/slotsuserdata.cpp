#include "slotsuserdata.h"

#include <slots/sql/slotusersaver.h>
#include <util/stringutil.h>
#include <util/timeutil.h>

BEGIN_NAMESPACE(slots)

const std::vector<std::string> SlotsUserData::UserInRedisFields = {
    SlotCacheStr::sName, // 0
    SlotCacheStr::sCountry,// 1
    SlotCacheStr::sAvatar,// 2
    SlotCacheStr::sSex,// 3
    SlotCacheStr::sLevel,// 4
    SlotCacheStr::sExp,// 5
    SlotCacheStr::sMoney,// 6
    SlotCacheStr::sVip,// 7
    SlotCacheStr::sVipPoint,// 8
    SlotCacheStr::sFreeGameTimes,// 9
    SlotCacheStr::sTinyGameEarned,// 10
    SlotCacheStr::sLastBet,// 11
    SlotCacheStr::sLastLines,// 12
    SlotCacheStr::sLastHall// 13
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

bool SlotsUserData::getByMid(const std::string &mid, SlotsUserPtr &out) {
    std::string uid;
    //get uid from cache
    if (getUidByMid(mid, uid)) {
        return getByUid(uid, out);
    }
    SlotsDB &db = SlotsDB::getInstance();
    // this user exist
    if(db.getUserIdByMachineId(mid, uid)) {
        setUidWithMid(mid, uid);
        return getByUid(uid, out);
    }
    if (out.get() == nullptr) {
        out.reset(new SlotsUser);
    }
    // init new user
    if (db.getUserInfoByMachineId(mid, *out)) {
        setUidWithMid(mid, out->uInfo.uid);
        return true;
    }
    return false;
}

bool SlotsUserData::getUserByMid(const std::string &mid, UserUnion &out) {
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
    //init new user
    if (db.initNewUser(mid, out)) {
        setUidWithMid(mid, out.uid);
        setUserToCache(out);
        return true;
    }
    return false;
}

bool SlotsUserData::getUserByUid(UserUnion &out){
    if (!getUserInCache(out)) {
	SlotsDB &db = SlotsDB::getInstance();
	//bool ret = db.getUserInfoByUserId(uid, out);
        bool ret = true;
	if (!ret) {
	    CLOG(WARNING) << "Get user[" << out.uid << "] info failed.";
	    return false;
	}
        setUserToCache(out);
	return true;
    }
    return true;
}

bool SlotsUserData::getByUid(const std::string &uid, SlotsUserPtr &out){
    if (_data.find(uid) == _data.end()) {
	SlotsDB &db = SlotsDB::getInstance();
	out.reset(new SlotsUser);
	bool ret = db.getUserInfoByUserId(uid, *out);
	if (!ret) {
	    CLOG(WARNING) << "Get user[" << uid << "] info failed.";
	    return false;
	}
	set(out->uInfo.uid, out);
	return true;
    }
    out = _data[uid];
    return true;
}

void SlotsUserData::set(const std::string &uid, SlotsUserPtr &in) {
    MUTEX_GUARD(_lock);
    _data[uid] = in;
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

bool SlotsUserData::getUserInCache(UserUnion &user) {
    // uid is key
    std::vector<std::string> result;
    auto ret =
        _redisClient.Hmget(user.uid, UserInRedisFields, &result);
    if (result.size() != UserInRedisFields.size() ) {
        CLOG(INFO) << "Invalid result for user info with userID:" << user.uid;
        return false;
    }
    user.name = result[0];
    user.country = result[1];
    user.avatar = result[2];
    user.isMale = (result[3] == "1");
    user.level = cgserver::StringUtil::StrToInt32WithDefault(result[4].data(), 0);
    user.exp = cgserver::StringUtil::StrToInt64WithDefault(result[5].data(), 0);
    user.fortune = cgserver::StringUtil::StrToInt64WithDefault(result[6].data(), 0);
    user.vipLevel = cgserver::StringUtil::StrToInt32WithDefault(result[7].data(), 0);
    user.vipPoint = cgserver::StringUtil::StrToInt32WithDefault(result[8].data(), 0);
    user.freeGameTimes = cgserver::StringUtil::StrToInt32WithDefault(result[9].data(), 0);
    user.tinyGameEarned = cgserver::StringUtil::StrToInt64WithDefault(result[10].data(), 0);
    user.lastBet = cgserver::StringUtil::StrToInt64WithDefault(result[11].data(), 0);
    user.lastLines = cgserver::StringUtil::StrToInt32WithDefault(result[12].data(), 0);
    user.lastHallID = cgserver::StringUtil::StrToInt32WithDefault(result[13].data(), 0);
    return true;
}

bool SlotsUserData::setUserToCache(UserUnion &user) {
    // uid is key
    std::vector<std::string> vals;
    vals.push_back(user.name);
    vals.push_back(user.country);
    vals.push_back(user.avatar);
    vals.push_back(user.isMale ? SlotCacheStr::sLRecvTrue: SlotCacheStr::sLRecvFalse);
    vals.push_back(cgserver::StringUtil::toString(user.level));
    vals.push_back(cgserver::StringUtil::toString(user.exp));
    vals.push_back(cgserver::StringUtil::toString(user.fortune));
    vals.push_back(cgserver::StringUtil::toString(user.vipLevel));
    vals.push_back(cgserver::StringUtil::toString(user.vipPoint));
    vals.push_back(cgserver::StringUtil::toString(user.freeGameTimes));
    vals.push_back(cgserver::StringUtil::toString(user.tinyGameEarned));
    vals.push_back(cgserver::StringUtil::toString(user.lastBet));
    vals.push_back(cgserver::StringUtil::toString(user.lastLines));
    vals.push_back(cgserver::StringUtil::toString(user.lastHallID));
    auto ret =
        _redisClient.Hmset(user.uid, UserInRedisFields, vals);
    if (ret != RC_SUCCESS) {
        CLOG(WARNING) << " set user to redis with userID:" << user.uid << " failed.";
        return false;
    }
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
