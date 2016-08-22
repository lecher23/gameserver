#ifndef BASICDATA_H
#define BASICDATA_H

#include <util/common_define.h>
#include <util/stringutil.h>
#include <map>
#include <vector>

BEGIN_NAMESPACE(slots)
#define RANK_LIMIT 1000
#define MAX_ELE_TYPES 16

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
    EGE_PLAYED_GAME // has play game
};

enum SlotsStyle{
    ESS_SLOTS_1 = 0,
    ESS_END
};

#define SLOTS_GAME_TYPES ESS_END

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
    
struct UserInfo{
    std::string uid;
    std::string mid;
    std::string fname;
    std::string avatar;
    std::string male; // 0 or 1
    std::string country;
    bool changed;
};
DF_SHARED_PTR(UserInfo);        

struct UserResource {
    std::string uid;
    int32_t level;
    int64_t exp;
    int64_t fortune;
    int32_t vipLevel;
    int64_t vipPoint;
    int32_t tmpVipLevel;
    int64_t tmpVipEndTime;	
    bool changed;

    void reset() {
	level = 1;
	exp = 0;
	fortune = 0;
	vipLevel = 0;
	vipPoint = 0;
	tmpVipLevel = 0;
	tmpVipEndTime = 0;
	changed = false;
    }

    void incrExp(int64_t input) {
	if (input <= 0) return;
	exp += input;
	changed = true;
    }

    void incrVipPoint(int64_t input) {
	if (input == 0) return;
	vipPoint += input;
	changed = true;
    }
	
    void levelUp() {
	level ++;
	changed = true;
    }

    void vipLevelUp(){
	vipLevel ++;
	changed = true;
    }

    void incrFortune(int64_t earned) {
	if (earned == 0)
	    return;
	fortune += earned;
	if (fortune < 0) fortune = 0;
	changed = true;
    }
};
DF_SHARED_PTR(UserResource);

struct UserHistory {
    std::string uid;
    int64_t maxFortune;
    int64_t maxEarned;
    int64_t totalEarned;
    int64_t totalBet;
    int64_t twEarned;
    int32_t lwEarnedSort;
    int32_t lwLevelSort;
    int32_t lwFortuneSort;
    int32_t lwAchievSort;
    bool changed;
    void reset() {
	maxFortune = 0;
	maxEarned = 0;
	totalEarned = 0;
	twEarned = 0;
	lwEarnedSort = 9999;
	lwLevelSort = 9999;
	lwFortuneSort = 9999;
	lwAchievSort = 9999;
	changed = false;
    }

    void incrBet(int64_t bet) {
	totalBet += bet;
	changed = true;
    }
    
    void newFortune(int64_t fortune) {
	if (fortune > maxFortune) {
	    maxFortune = fortune;
	    changed = true;
	}
    }
    void newEarned(int64_t earned) {
	if (maxEarned < earned) {
	    maxEarned = earned;
	    changed = true;
	}
    }
	
    void incrEarned(int64_t earned) {
	if (earned == 0) return;
	totalEarned += earned;
	twEarned += earned;
	changed = true;
    }
	
};

struct GameHistory{
    std::string uid;
    int32_t friendNum;
    int32_t friendGiftsNum;
    int32_t consitiveLogin;
    int32_t tinyGameTimes;
    int32_t bigwin[SLOTS_GAME_TYPES];// 0,0,0,0
    int32_t megawin[SLOTS_GAME_TYPES];// 0,0,0,0
    int32_t freeTimes[SLOTS_GAME_TYPES];
    int32_t gameTimes[SLOTS_GAME_TYPES];
    int32_t jackpotTimes[SLOTS_GAME_TYPES];
    int32_t gJackpotTimes;
    int32_t fourLine[SLOTS_GAME_TYPES][MAX_ELE_TYPES]; // [gametype][ele]
    int32_t fiveLine[SLOTS_GAME_TYPES][MAX_ELE_TYPES]; // [gametype][ele]
    bool changed;
    void reset() {
	uid = uid;
	friendNum = 0;
	friendGiftsNum = 0;
	consitiveLogin = 0;
	tinyGameTimes = 0;
	gJackpotTimes = 0;
	for (size_t i = 0; i < SLOTS_GAME_TYPES; ++i){
	    bigwin[i] = 0;
	    megawin[i] = 0;
	    freeTimes[i] = 0;
	    gameTimes[i] = 0;
	    jackpotTimes[i] = 0;
	    for (size_t j = 0; j < MAX_ELE_TYPES; ++j){
		fourLine[i][j] = 0;
		fiveLine[i][j] = 0;
	    }
	}
	changed = true;
    }

    bool deserialize(std::vector<std::string> &row) {
	if (row.size() < 13) return false;
	uid = row[0];
	friendNum = cgserver::StringUtil::StrToInt32WithDefault(row[1].c_str(), 0);
	friendGiftsNum = cgserver::StringUtil::StrToInt32WithDefault(row[2].c_str(), 0);
	consitiveLogin = cgserver::StringUtil::StrToInt32WithDefault(row[3].c_str(), 0);
	tinyGameTimes = cgserver::StringUtil::StrToInt32WithDefault(row[4].c_str(), 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[5].c_str(), ',', bigwin, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[6].c_str(), ',', megawin, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[7].c_str(), ',', freeTimes, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[8].c_str(), ',', gameTimes, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[9].c_str(), ',', jackpotTimes, SLOTS_GAME_TYPES, 0);
	gJackpotTimes = cgserver::StringUtil::StrToInt32WithDefault(row[10].c_str(), 0);
	int32_t *pFourLine[SLOTS_GAME_TYPES];
	int32_t *pFiveLine[SLOTS_GAME_TYPES];
	for (size_t i = 0; i < SLOTS_GAME_TYPES; ++i) {
	    pFourLine[i] = fourLine[i];
	    pFiveLine[i] = fiveLine[i];
	}
	cgserver::StringUtil::StrToIntArray
	    (row[11].c_str(), ';', ',', pFourLine, SLOTS_GAME_TYPES,MAX_ELE_TYPES);
	cgserver::StringUtil::StrToIntArray
	    (row[12].c_str(), ';', ',', pFiveLine, SLOTS_GAME_TYPES,MAX_ELE_TYPES);
	
	return true;
    }
};

DF_SHARED_PTR(GameHistory);

struct SlotsUser{
    UserInfo uInfo;
    UserResource uRes;
    UserHistory uHis;
    GameHistory gDetail;
};
DF_SHARED_PTR(SlotsUser);

/*shared by other user.*/
struct SlotsMail{
    std::string mailId;
    std::string title;
    std::string content;
    int32_t attachment;
    std::string createTime;
    int32_t keepDays;
};
DF_SHARED_PTR(SlotsMail);

struct Attachment{
    int32_t type;
    int64_t value;
};
DF_SHARED_PTR(Attachment);    

/*owned by one user*/
struct UserMail{
    SlotsMail mailInfo;
    bool bRead;
    bool bGet;
    bool bDel;
};
DF_SHARED_PTR(UserMail);

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

/** Rank data **/

struct LeaderBoardItem{
    int64_t uid;
    std::string name;
    int32_t country;
    int64_t value;
};
DF_SHARED_PTR(LeaderBoardItem);

struct  LeaderBoardRank {
    std::vector<LeaderBoardItemPtr> data;
    int64_t timestamp;
    LeaderBoardRank() {
	data.resize(RANK_LIMIT);
	data.clear();
	timestamp = 0;
    }
};

struct LeaderBoardData {
    LeaderBoardRank level;
    LeaderBoardRank fortune;
    LeaderBoardRank earned;
    LeaderBoardRank achievement;
};

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

struct SlotsEventData{
    int64_t earned;
    int64_t bet;
    std::vector<ResultType> retTypes;
    std::string detail;
    bool enableTinyGame;
    bool isFreeRound;
    SlotsStyle gType;
};

typedef SlotsEventData SingleGameDetail;

struct UserCJ {
    std::string uid;
    std::string aid;
    bool isRecvReward;
    int64_t progress;
    bool isGain;
    int64_t time;
    bool changed;
    int fieldsNum() {
	return 6;
    }
};
DF_SHARED_PTR(UserCJ);
typedef std::vector<UserCJPtr> UserCJs;

struct CjSetting{
    std::string id;
    int64_t target;
    int64_t reward;
    int32_t reward_type;
    int32_t type;
    int fieldsNum() {
	return 5;
    }
};
DF_SHARED_PTR(CjSetting);
typedef std::vector<CjSettingPtr> CjSettings;
typedef std::map<int32_t, CjSettings> CjSettingMap;

typedef UserCJPtr AchievementPtr;
typedef std::vector<AchievementPtr> Achievements;


END_NAMESPACE
#endif
