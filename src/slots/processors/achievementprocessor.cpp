#include "achievementprocessor.h"

BEGIN_NAMESPACE(slots)

AchievementProcessor::AchievementProcessor(){
}

AchievementProcessor::~AchievementProcessor(){
}

bool AchievementProcessor::process(GameContext &context) const {
    processLevel(context);
    processFriends(context);
    processMoney(context);
    processGame(context);
    return true;
}

void AchievementProcessor::processLevel(GameContext &context) const {
    auto userLevel = context.user->uRes.level;
    if(context.events.count(EGE_LEVEL_UP) == 0) {
	return;
    }
    if (userLevel == 10) {
	// achivement event;
    }
}

void AchievementProcessor::processFriends(GameContext &context) const {
    auto &history = context.user->gDetail;
    auto &events = context.events;
    if (events.count(EGE_NEW_FRIEND) != 0) {
	// frends number of user is xxx
    }
    if (events.count(EGE_RECV_GIFT) != 0) {
	// gift sent times is xxx
    }	
    if (events.count(EGE_RECV_GIFT) != 0) {
	// gift recieve times is xxx
    }
}

void AchievementProcessor::processMoney(GameContext &context) const {
    auto &events = context.events;
    if (events.count(EGE_USE_BET) != 0) {
	// total bet money
    }
    if (events.count(EGE_EARNED_INCR) != 0) {
	// total earned money
    }
}

void AchievementProcessor::processGame(GameContext &context) const {
    if(context.events.count(EGE_PLAYED_GAME) == 0) {
	return;
    }
    // tinye game times

    // small game times

    // four line times in game x with elements i

    // five line times in game x with elements i
}

END_NAMESPACE
