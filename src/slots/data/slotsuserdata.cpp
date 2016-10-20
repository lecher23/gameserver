#include "slotsuserdata.h"

#include <slots/sql/slotusersaver.h>
#include <util/stringutil.h>
#include <util/timeutil.h>

BEGIN_NAMESPACE(slots)

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
    SlotsDB &db = SlotsDB::getInstance();
    // this user exist
    if(db.getUserIdByMachineId(mid, uid)) {
        return getByUid(uid, out);
    }
    if (out.get() == nullptr) {
        out.reset(new SlotsUser);
    }
    return db.getUserInfoByMachineId(mid, *out);
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
	out->uInfo.changed = false;
	out->uRes.changed = false;
	out->uHis.changed = false;
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
#undef PARSE_CACHE_VAL_TO_INT32

#undef GENERATE_LOGIN_KEY
#undef REDIS_EASY_HSET
#undef REDIS_EASY_EXPIREAT

END_NAMESPACE
