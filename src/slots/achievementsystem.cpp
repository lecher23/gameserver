#include "achievementsystem.h"

BEGIN_NAMESPACE(slots)

AchievementSystem::AchievementSystem(){
}

AchievementSystem::~AchievementSystem(){
}

void AchievementSystem::levelUP(SlotsUserPtr user, SlotsEventData &data){
    if (user->uRes.level == 10) {
	// add new achievement to user.
    }
}

void AchievementSystem::fortuneChange(const std::string &uid, int64_t before, int64_t after){
    if (before < 100 && after >=100) {
	// add new achievement to user
    }
}

void AchievementSystem::gameDetailChange(SlotsUserPtr user) {
    auto &ud = user->gDetail;
    for (size_t i = 0; i < SLOTS_GAME_TYPES; ++i) {
	if (ud.bigwin[i] == 10) {
	    CLOG(INFO) << "10 TIMES OF BIG WIN.";
	}else if (ud.megawin[i] == 10) {
	    CLOG(INFO) << "10 TIMES OF MEGA WIN.";
	}else if (ud.megawin[i] == 10) {
	    CLOG(INFO) << "10 TIMES OF MEGA WIN.";
	}
    }
}

END_NAMESPACE
