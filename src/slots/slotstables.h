#ifndef SLOTSTABLES_H
#define SLOTSTABLES_H

#include "../util/common_define.h"

namespace slots{
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
    
    struct SlotsUser{
	UserInfo uInfo;
	UserResource uRes;
	UserHistory uHis;
    };
    DF_SHARED_PTR(SlotsUser);    

    /*shared by other user.*/
    struct SlotsMail{
	std::string mailId;
	std::string title;
	std::string content;
	std::string attachment;
	std::string createTime;
	std::string keepDays;
    };
    DF_SHARED_PTR(SlotsMail);

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

    struct LeaderBoardItem{
	std::string i;
	std::string name;
	std::string country;
	std::string level;
	std::string value;
    };
    DF_SHARED_PTR(LeaderBoardItem);
}
#endif
