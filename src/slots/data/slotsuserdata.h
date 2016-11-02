#ifndef SLOTSUSERDATA_H
#define SLOTSUSERDATA_H

#include <slots/sql/slotsdb.h>
#include <slots/data/gamecontext.h>
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

  const std::string sName = "i:nm";
  const std::string sCountry = "i:cty";
  const std::string sAvatar = "i:avt";
  const std::string sSex = "i:male";

  const std::string sLevel = "r:level";
  const std::string sExp = "r:exp";
  const std::string sMoney = "r:money";
  const std::string sVip = "r:vip";
  const std::string sVipPoint = "r:vpt";

  const std::string sMaxFortune = "h:mf";
  const std::string sMaxEarned = "h:me";
  const std::string sTotalEarned = "h:te";
  const std::string sTotalBet = "h:tb";
  const std::string sLastLogin = "h:ll";
  const std::string sLoginDays = "h:ld";
  const std::string sJackpot = "h:jp";

  const std::string sBigWin = "t:bw:";
  const std::string sMegaWin = "t:mw:";
  const std::string sSuperWin = "t:sw:";
  const std::string sGameCount = "t:gt:";
  const std::string sFreeGameCount = "t:fg:";
  const std::string sTinyGameCount = "t:tg:";
  const std::string sLink = "t:lk:";

  const std::string sFreeGameTimes = "g:fgt";
  const std::string sTinyGameEarned = "g:prz";
  const std::string sLastBet = "g:bet";
  const std::string sLastLines = "g:lines";
  const std::string sLastHall = "g:hid";
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

    bool getContextForGame(GameContext &user);
    bool setContextForGame(GameContext &user);
    bool getContextForLogin(GameContext &user);
    bool updateLoginInfo(const std::string &uid, int64_t ts);
    bool getUserByMid(const std::string &mid, GameContext &out);
    bool getUserByUid(GameContext &out);
    int64_t getTinyGameReward(const std::string &uid);
    bool checkAchievement(const std::string &uid, const std::string &cjID);

    void setDailyReward(const std::string &userID, const LoginReward &in);
    void updateDailyReward(const std::string &userID, bool recved);
    bool getDailyReward(const std::string &userID, LoginReward &out);

    int32_t incrOnlineTime(const std::string &userID, int32_t incrVal);
    bool getOnlineInfo(
        const std::string &userID, OnlineInfo &onlineInfo, int64_t defaultReward);
    bool setOnlineInfo(const std::string &userID, OnlineInfo &onlineINfo);
    void recvOnlineGift(const std::string &userID, bool recved);

    bool setUserToCache(GameContext &user); // rewrite it
    bool addUserFortuneInCache(const std::string &uid, int64_t incr, int64_t &res);
    bool getUidByMid(const std::string &mid, std::string &uid);

private:
    void setUidWithMid(const std::string &mid, std::string &uid);

    std::map<std::string, SlotsUserPtr> _data;
    std::mutex _lock;
    CRedisClient &_redisClient;

    static const std::vector<std::string> RedisGameInfoKeys;
    static const std::vector<std::string> RedisLoginInfoKeys;
};

DF_SHARED_PTR(SlotsUserData);

END_NAMESPACE
#endif
