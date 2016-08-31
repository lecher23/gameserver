#ifndef BASICDATA_H
#define BASICDATA_H

#include <util/common_define.h>
#include <util/stringutil.h>
#include <slots/model/userinfo.h>
#include <slots/model/userhistory.h>
#include <slots/model/userresource.h>
#include <slots/model/gamehistory.h>
#include <slots/model/usermail.h>
#include <slots/model/mailinfo.h>
#include <slots/model/rankinfo.h>
#include <slots/model/userachievement.h>
#include <slots/model/userachievementconfig.h>
#include <slots/model/loginrewardconfig.h>
#include <slots/model/cargohistory.h>
#include <slots/model/cargoinfo.h>
#include <map>
#include <vector>

BEGIN_NAMESPACE(slots)
#define TO_LINE_CJ_VALUE(gameid, eleid, line, value) (gameid * 1000000000000 + eleid * 1000000000 + line * 10000000 + value)
#define TO_GAME_CJ_VALUE(gameid, value) (gameid * 10000000 + value)
#define CHANCE_MAX_POINT 1000

template <typename T>
struct BasicData {
    T val;
    bool changed;

    void setVal(T &val_) {
	val = val_;
	changed = true;
    }
};

enum GameEvent {
    EGE_LEVEL_UP = 0, // level up
    EGE_EARNED_INCR, // increment total earned
    EGE_USE_BET, // use bet
    EGE_NEW_FRIEND, // add new friend success
    EGE_RECV_GIFT, // get gift from user
    EGE_SEND_GIFT, // send gift to user
    EGE_PLAYED_GAME, // has play game
    EGE_TINY_GAME,
    EGE_FREE_GAME,
    EGE_MEGA_WIN,
    EGE_BIG_WIN,
    EGE_JACKPOT,
    EGE_LINE,
    EGE_GAME_COUNT,
    EGE_LOGIN
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
    int64_t runnerReward;
    int64_t daysReward;
    int64_t specialReward;
    int64_t timestamp;
    bool recved;
LoginReward():runnerReward(0),daysReward(0),specialReward(0),timestamp(0){}
    void setRunnerReward(int64_t val) {
        runnerReward = val;
    }
    void setDaysReward(int64_t val) {
        daysReward = val;
    }
    void setSpecialReward(int64_t val) {
        specialReward = val;
    }
    void setRecved(bool val){
        recved = val;
    }
};

struct SlotsUser{
    UserInfo uInfo;
    UserResource uRes;
    UserHistory uHis;
    GameHistory gDetail;
    LoginReward loginReward;
};
DF_SHARED_PTR(SlotsUser);

/*owned by one user*/
struct FHistory{
    std::string uid;
    int32_t inviteCount;
    int64_t totalReward;
    int64_t rewardRemain;
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
    ERT_TINY_GAME,
    ERT_FOULINE = 100,
    ERT_FOULINE_1,
    ERT_FOULINE_2,
    ERT_FOULINE_3,
    ERT_FOULINE_4,
    ERT_FOULINE_5,
    ERT_FOULINE_6,
    ERT_FOULINE_7,
    ERT_FOULINE_8,
    ERT_FOULINE_9,
    ERT_FIVELINE = 200,
    ERT_FIVELINE_1,
    ERT_FIVELINE_2,
    ERT_FIVELINE_3,
    ERT_FIVELINE_4,
    ERT_FIVELINE_5,
    ERT_FIVELINE_6,
    ERT_FIVELINE_7,
    ERT_FIVELINE_8,
    ERT_FIVELINE_9,
    ERT_MULTILINE_END
};

struct GameRecord{
    std::string uid;
    SlotsStyle gameType;
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

struct SlotLineInfo {
    int32_t colum;
    int32_t count;
    int32_t ele;
};

struct SlotsEventData{
    int64_t earned;
    int64_t bet;
    std::vector<ResultType> retTypes;
    std::vector<SlotLineInfo> lineInfo;
    std::string detail;
    bool enableTinyGame;
    bool isFreeRound;
    SlotsStyle gType;
};

typedef SlotsEventData SingleGameDetail;

typedef UserCJPtr AchievementPtr;
typedef std::vector<AchievementPtr> Achievements;

END_NAMESPACE
#endif
