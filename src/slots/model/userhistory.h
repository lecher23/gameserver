#ifndef USERHISTORY_H
#define USERHISTORY_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct UserHistory{
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
        totalBet = 0;
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
    UserHistory() {
        reset();
    }
};

END_NAMESPACE
#endif
