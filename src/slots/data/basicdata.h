#ifndef BASICDATA_H
#define BASICDATA_H

#include <util/common_define.h>
#include <util/stringutil.h>
#include <slots/model/userinfo.h>
#include <slots/model/userresource.h>
#include <slots/model/gamehistory.h>
#include <slots/model/usermail.h>
#include <slots/model/mailinfo.h>
#include <slots/model/rankinfo.h>
#include <slots/model/userachievement.h>
#include <slots/model/cargohistory.h>
#include <slots/model/cargoinfo.h>
#include <slots/data/gameresult.h>
#include <map>
#include <vector>

BEGIN_NAMESPACE(slots)
#define TO_LINE_CJ_VALUE(gameid, eleid, line, value) (gameid * 1000000000000 + eleid * 1000000000 + line * 10000000 + value)
#define TO_GAME_CJ_VALUE(gameid, value) (gameid * 10000000 + value)

const std::string sMysqlTrue = "1";
const std::string sMysqlFalse = "0";

template <typename T>
struct BasicData {
    T val;
    bool changed;

    void setVal(T &val_) {
	val = val_;
	changed = true;
    }
};

enum RankType{
    ERT_CUR_FORTUNE = 0,
    ERT_CUR_EARNED,
    ERT_CUR_ACHIVEMENTS,
    ERT_CUR_LEVEL,
    ERT_LW_FORTUNE,
    ERT_LW_EARNED,
    ERT_LW_ACHIVEMENTS,
    ERT_LW_LEVEL,
    ERT_TW_EARNED,
    ERT_END
};

struct LoginReward {
    int32_t runnerIdx{0};
    int32_t runnerReward{0};
    int32_t vipExtra{0};
    bool recved{false};
    int32_t dayReward{0};
    int32_t dayN{0};
};

struct SlotsUser{
  UserInfo uInfo;
  UserResource uRes;
  GameHistory uHis;
  Achievements uCj;
  GameStatus gSt;

  bool deserialize(const std::vector<std::string> &row) {
    if (row.size() < 15) {
      CLOG(WARNING) << "Ivalid colum number.";
      return false;
    }
    uInfo.uid = row[0];
    uInfo.mid = row[1];
    uInfo.fname = row[2];
    uInfo.avatar = row[4];
    uInfo.male = row[5];
    uInfo.country = row[6];
    uRes.uid = uInfo.uid;
    bool ret = cgserver::StringUtil::StrToInt32(row[8].c_str(), uRes.level);
    ret = ret && cgserver::StringUtil::StrToInt64(row[9].c_str(), uRes.exp);
    ret = ret && cgserver::StringUtil::StrToInt64(row[10].c_str(), uRes.fortune);
    ret = ret && cgserver::StringUtil::StrToInt32(row[11].c_str(), uRes.vipLevel);
    if(!ret) {
      CLOG(WARNING) << "Invalid number.";
    }
    return ret;
  }
};
DF_SHARED_PTR(SlotsUser);

/*owned by one user*/
struct FHistory{
    std::string uid;
    int32_t inviteCount;
    int64_t totalReward;
    int64_t rewardRemain;
    bool deserialize(const std::vector<std::string> &row) {
        if (row.size() < 4) {
            CLOG(WARNING) << "Invalid colum number";
        }
        uid = row[0];
        bool ret = cgserver::StringUtil::StrToInt32(row[1].c_str(), inviteCount);
        ret = ret && cgserver::StringUtil::StrToInt64(row[2].c_str(), totalReward);
        ret = ret && cgserver::StringUtil::StrToInt64(row[3].c_str(), rewardRemain);
        if (!ret) {
            CLOG(WARNING) << "Invalid number";
        }
        return ret;
    }
};
DF_SHARED_PTR(FHistory);

/* user mails*/
typedef std::vector<UserMailPtr> UserMails;
typedef std::vector<SlotsUserPtr> SlotsUsers;
typedef SlotsUsers FriendsList;

/** Game data during running.**/

enum ResultType {
    ERT_NORMAL = 0,
    ERT_BIG_WIN,
    ERT_MEGA_WIN,
    ERT_JACKPOT,
    ERT_TINY_GAME
};

struct GameRecord{
    std::string uid;
    ThemeTypes gameType;
    int64_t time;
    std::string detail;
    int64_t bet;
    int64_t earned;
};

// game scene data
struct GameSceneData{
    int64_t bet;
    int32_t times;
    int32_t remain;
};
DF_SHARED_PTR(GameSceneData);

typedef std::map<int64_t, GameSceneDataPtr> GameSceneDataSet;

class PersistenceBase{
 public:
    virtual ~PersistenceBase() {}
    virtual void save2MySQL(uint64_t factor) = 0;
    virtual bool needSave(uint64_t factor) {};
 protected:
    int64_t lastActiveTime;
};

DF_SHARED_PTR(PersistenceBase);

END_NAMESPACE
#endif
