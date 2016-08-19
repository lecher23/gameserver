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
    if (userLevel == 10) {
	// achivement event;
    }
}

void AchievementProcessor::processFriends(GameContext &context) const {
    auto &history = context.user->gDetail;
    // frends number of user is xxx

    // gift sent times is xxx

    // gift recieve times is xxx
}

void AchievementProcessor::processMoney(GameContext &context) const {
    // total bet money

    // total earned money
}

void AchievementProcessor::processGame(GameContext &context) const {
    // tinye game times

    // small game times

    // four line times in game x with elements i

    // five line times in game x with elements i
}

END_NAMESPACE
