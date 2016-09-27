#include "gameresultprocessor.h"
#include <slots/slotsconfig.h>

BEGIN_NAMESPACE(slots)

GameResultProcessor::GameResultProcessor(){
}

GameResultProcessor::~GameResultProcessor(){
}

bool GameResultProcessor::process(GameContext &context) const {
    context.events.push_back(EventInfo(EGE_PLAYED_GAME));
    //processHall(); force to win prize.update hall prize pool
    processGameDetail(context, context.gameInfo);
    // is free to play
    if (!context.gameInfo.bFreeGame) {
	context.events.push_back(EventInfo(EGE_USE_BET, context.gameInfo.bet));
    }
    // update user fortune
    processMoney(context, context.gameInfo);
    // update user exp&level
    processExp(context, context.gameInfo);
    return true;
}


void GameResultProcessor::processGameDetail(GameContext &context, GameResult &data) const {
    auto &udt = context.user->gDetail;
    auto gType = data.gType;
    // incr game times
    ++udt.gameTimes[gType];
    context.events.push_back(
	EventInfo(EGE_GAME_COUNT, TO_GAME_CJ_VALUE(gType, udt.gameTimes[gType])));
    if(data.bBigwin) {
        auto &bigwinVal = udt.bigwin[gType];
        ++ bigwinVal;
        context.events.push_back(
            EventInfo(EGE_BIG_WIN, TO_GAME_CJ_VALUE(gType, bigwinVal)));
    }
    if(data.bMegawin) {
        auto &megawinVal = udt.megawin[gType];
        ++megawinVal;
        context.events.push_back(
            EventInfo(EGE_MEGA_WIN, TO_GAME_CJ_VALUE(gType, megawinVal)));
    }
    if(data.bJackpot1) {
        auto &jackVal = udt.jackpotTimes[data.gType];
        udt.gJackpotTimes ++;
        ++jackVal;
        context.events.push_back(
            EventInfo(EGE_JACKPOT, TO_GAME_CJ_VALUE(gType, jackVal)));
    }
    if (data.tinyGameEleCount >= 3) {
	udt.tinyGameTimes ++;
    }
    udt.changed = true;
}

void GameResultProcessor::processExp(GameContext &context, GameResult &data) const {
    auto &uRes = context.user->uRes;
    // if zero bet then exp will not change
    if (data.bFreeGame) {
	return;
    }
    auto &slotsConfig = SlotsConfig::getInstance();
    auto expGot = slotsConfig.expGain(context, data.bet);
    uRes.incrExp(expGot);
    int64_t expNeed = 0;
    while(true) {
        expNeed = slotsConfig.expNeedForLevelUp(uRes.level);
        if (expNeed < 0 || expNeed > uRes.exp ) {
            return;
        }
        uRes.levelUp();
        context.events.push_back(EventInfo(EGE_LEVEL_UP, uRes.level));
    }
}

void GameResultProcessor::processMoney(GameContext &context, GameResult &data) const {
    UserHistory &uHis = context.user->uHis;
    uHis.incrBet(data.bet);
    int64_t actualEarned = data.earned - data.bet;
    if (actualEarned == 0) {
	return;
    }
    UserResource &uRes = context.user->uRes;
    uRes.incrFortune(actualEarned);
    // update max fortune
    uHis.newFortune(uRes.fortune);
    // update max earned
    uHis.newEarned(data.earned);
    // update earned (include this week and total)
    auto pre = uHis.totalEarned;
    uHis.incrEarned(data.earned);
    if (pre != uHis.totalEarned) {
	// if total earned changed, create event.
	context.events.push_back(EventInfo(EGE_EARNED_INCR, pre, uHis.totalEarned));
    }
}

END_NAMESPACE
