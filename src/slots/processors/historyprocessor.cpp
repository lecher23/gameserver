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
    if (context.gameInfo.isFreeRound) {
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


void HistoryProcessor::processGameDetail(GameContext &context, SingleGameDetail &data) const {
    auto &udt = context.user->gDetail;
    auto gType = data.gType;
    // get all types
    // here we just use one event that: game played.
    for (auto itr: data.retTypes) {
	switch(itr) {
	case ERT_BIG_WIN:{
	    auto &bigwinVal = udt.bigwin[gType];
	    ++ bigwinVal;
	    context.events.push_back(
		EventInfo(EGE_BIG_WIN, TO_GAME_CJ_VALUE(gType, bigwinVal)));
	    break;
	}
	case ERT_MEGA_WIN:{
	    auto &megawinVal = udt.megawin[gType];
	    ++megawinVal;
	    context.events.push_back(
		EventInfo(EGE_MEGA_WIN, TO_GAME_CJ_VALUE(gType, megawinVal)));	    
	    break;
	}
	case ERT_JACKPOT:{
	    auto &jackVal = udt.jackpotTimes[data.gType];
	    udt.gJackpotTimes ++;
	    ++jackVal;
	    context.events.push_back(
		EventInfo(EGE_JACKPOT, TO_GAME_CJ_VALUE(gType, jackVal)));
	    break;
	}
	default:
	    ;
	}
    }
    for (auto &item: data.lineInfo) {
	auto ele = item.ele;
	if (item.colum == 4) {
	    auto &now = udt.fourLine[gType][ele];
	    auto pre = now;
	    now += item.count;
	    context.events.push_back(
		EventInfo(EGE_LINE, TO_LINE_CJ_VALUE(gType, ele, pre),
			  TO_LINE_CJ_VALUE(gType, ele, now)));
	}else if (item.colum == 5) {
	    auto &now = udt.fiveLine[gType][ele];
	    auto pre = now;
	    now += item.count;
	    context.events.push_back(
		EventInfo(EGE_LINE, TO_LINE_CJ_VALUE(gType, ele, pre),
			  TO_LINE_CJ_VALUE(gType, ele, now)));
	}
    }
    if (data.enableTinyGame) {
	udt.tinyGameTimes ++;
    }
    udt.changed = true;
}

void HistoryProcessor::processExp(GameContext &context, SingleGameDetail &data) const {
    auto &uRes = context.user->uRes;    
    // if zero bet then exp will not change
    if (data.bet == 0) {
	return;
    }
    auto &config = SlotsConfig::getInstance();
    auto expNeed = config.expNeed2LevelUp(uRes.exp);
    auto expGot = config.expGain(data.bet);
    // level up event
    if (expNeed <= expGot ) {
	uRes.levelUp();
	context.events.push_back(EventInfo(EGE_LEVEL_UP, uRes.level));
    }
    uRes.incrExp(expGot);
}

void HistoryProcessor::processMoney(GameContext &context, SlotsEventData &data) const {
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
