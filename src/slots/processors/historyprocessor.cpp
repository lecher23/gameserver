#include "historyprocessor.h"
#include <slots/slotsconfig.h>

BEGIN_NAMESPACE(slots)

HistoryProcessor::HistoryProcessor(){
}

HistoryProcessor::~HistoryProcessor(){
}

bool HistoryProcessor::process(GameContext &context) const {
    context.events.push_back(EventInfo(EGE_PLAYED_GAME));
    processGameDetail(context, context.gameInfo);
    // is free to play
    if (context.gameInfo.freeGameTimes == 0) {
	context.gameInfo.bet = 0;
    } else {
	context.events.push_back(EventInfo(EGE_USE_BET, context.gameInfo.bet));
    }
    // update user fortune
    processMoney(context, context.gameInfo);
    // update user exp&level
    processExp(context, context.gameInfo);
    return true;
}


void HistoryProcessor::processGameDetail(GameContext &context, GameResult &data) const {
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

void HistoryProcessor::processExp(GameContext &context, GameResult &data) const {
    auto &uRes = context.user->uRes;
    // if zero bet then exp will not change
    if (data.bet == 0) {
	return;
    }
    // TODO: exp got setting
    auto expGot = data.bet;
    uRes.incrExp(expGot);
    auto &levelConfig = SlotsConfig::getInstance().levelConfig;
    int64_t expNeed = 0;
    while(true) {
        auto itr = levelConfig.find(uRes.level);
        if (itr == levelConfig.end()) {
            return;
        }
        expNeed = itr->second.expNeed;
        if (expNeed > uRes.exp ) {
            return;
        }
        uRes.levelUp();
        context.events.push_back(EventInfo(EGE_LEVEL_UP, uRes.level));
    }
}

void HistoryProcessor::processMoney(GameContext &context, GameResult &data) const {
    int64_t actualEarned = data.earned - data.bet;
    if (actualEarned == 0) {
	return;
    }
    UserResource &uRes = context.user->uRes;
    UserHistory &uHis = context.user->uHis;
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
