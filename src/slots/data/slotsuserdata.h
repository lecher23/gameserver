#ifndef SLOTSUSERDATA_H
#define SLOTSUSERDATA_H

#include <slots/sql/slotsdb.h>
#include <cache/redisclientfactory.h>

BEGIN_NAMESPACE(slots)

namespace SlotCacheStr{
  const std::string sDailyKeyPrefix = "L:";
  const std::string sMachineIDPrefix = "M:";
  const std::string sLRunnerIdKey = "rID";
  const std::string sLRunnerRewardKey = "rRwd";
  const std::string sLRecvKey = "recv";
  const std::string sLVipKey = "vip";
  const std::string sLDayKey = "dRwd";
  const std::string sLRecvTrue = "1";
  const std::string sLRecvFalse = "0";
  const std::string sOLTime = "ol:time";
  const std::string sOLLevel = "ol:level";
  const std::string sOLRecv = "ol:recv";
  const std::string sOLReward = "ol:rwd";
  const std::string sName = "name";
  const std::string sCountry = "country";
  const std::string sAvatar = "avatar";
  const std::string sSex = "sex";
  const std::string sLevel = "level";
  const std::string sExp = "exp";
  const std::string sMoney = "money";
  const std::string sVip = "vip";
  const std::string sVipPoint = "vpoint";
  const std::string sFreeGameTimes = "fgt";
  const std::string sTinyGameEarned = "tg:prz";
  const std::string sLastBet = "bet";
  const std::string sLastLines = "lines";
  const std::string sLastHall = "hall:id";
};

struct UserUnion{
  std::string name;
  std::string avatar;
  std::string country;
  bool gender;// 1: male
  int32_t level;
  int32_t vipLevel;
  int32_t vipPoint;
  int32_t freeGameTimes;
  int32_t lastLines;
  int32_t lastHallID;
  int64_t lastBet;
  int64_t tinyGameEarned;
  int64_t fortune;
  int64_t exp;
};

struct OnlineInfo{
    int32_t rewardLevel{0};// target reward level
    bool recved{true};
    int64_t rewardValue{0};
};

class SlotsUserData: public PersistenceBase{
 public:
    SlotsUserData();
    virtual ~SlotsUserData();
    virtual void save2MySQL(uint64_t factor);
    virtual bool needSave(uint64_t factor);

    bool getByUid(const std::string &uid, SlotsUserPtr &out);
    bool getByMid(const std::string &mid, SlotsUserPtr &out);
    void set(const std::string &uid, SlotsUserPtr &in);

    void setDailyReward(const std::string &userID, const LoginReward &in);
    void updateDailyReward(const std::string &userID, bool recved);
    bool getDailyReward(const std::string &userID, LoginReward &out);

    int32_t incrOnlineTime(const std::string &userID, int32_t incrVal);
    bool getOnlineInfo(
        const std::string &userID, OnlineInfo &onlineInfo, int64_t defaultReward);
    bool setOnlineInfo(const std::string &userID, OnlineInfo &onlineINfo);
    void recvOnlineGift(const std::string &userID, bool recved);

    bool getUserInCache(std::string &uid, UserUnion &user);

private:
    bool getUidByMid(const std::string &mid, std::string &uid);
    void setUidWithMid(const std::string &mid, std::string &uid);

    std::map<std::string, SlotsUserPtr> _data;
    std::mutex _lock;

    CRedisClient &_redisClient;
};

DF_SHARED_PTR(SlotsUserData);

END_NAMESPACE
#endif
