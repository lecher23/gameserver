#include "historyprocessor.h"
#include <slots/slotsconfig.h>

BEGIN_NAMESPACE(slots)

HistoryProcessor::HistoryProcessor(){
}

HistoryProcessor::~HistoryProcessor(){
}

bool HistoryProcessor::process(GameContext &context) const {
    processGameDetail(context.user, context.gameInfo);
    // is free to play
    if (context.gameInfo.isFreeRound) {
	context.gameInfo.bet = 0;
    }
    // update user fortune
    processMoney(context.user, context.gameInfo);
    // update user exp&level    
    processExp(context.user, context.gameInfo);
    // update game detail
    processGameDetail(context.user, context.gameInfo);
    return true;
}


void HistoryProcessor::processGameDetail(SlotsUserPtr &user, SingleGameDetail &data) const {
    auto &udt = user->gDetail;
    // get all types
    for (auto itr: data.retTypes) {
	switch(itr) {
	case ERT_BIG_WIN:
	    udt.bigwin[data.gType] ++;
	    break;
	case ERT_MEGA_WIN:
	    udt.megawin[data.gType] ++;
	    break;
	case ERT_JACKPOT:
	    udt.gJackpotTimes ++;
	    udt.jackpotTimes[data.gType] ++;
	    break;
	case ERT_NORMAL:
	    // do noting
	    break;
	default:
	    if (itr > ERT_FOULINE) {
		if (itr < ERT_FIVELINE) {
		    uint32_t eleNum = itr - ERT_FOULINE - 1;
		    udt.fourLine[data.gType][eleNum] ++;
		}else if(itr < ERT_MULTILINE_END) {
		    uint32_t eleNum = itr - ERT_FIVELINE - 1;
		    udt.fourLine[data.gType][eleNum] ++;
		}
	    }
	}
    }
    if (data.enableTinyGame) {
	udt.tinyGameTimes ++;
    }
    udt.changed = true;
}

void HistoryProcessor::processExp(SlotsUserPtr &user, SingleGameDetail &data) const {
    auto &uRes = user->uRes;    
    // if zero bet then exp will not change
    if (data.bet == 0) {
	return;
    }
    auto &config = SlotsConfig::instance();
    auto expNeed = config.expNeed2LevelUp(uRes.exp);
    auto expGot = config.expGain(data.bet);
    // level up event
    if (expNeed <= expGot ) {
	uRes.levelUp();
    }
    uRes.incrExp(expGot);
}

void HistoryProcessor::processMoney(SlotsUserPtr &user, SlotsEventData &data) const {
    int64_t actualEarned = data.earned - data.bet;
    if (actualEarned == 0) {
	return;
    }
    UserResource &uRes = user->uRes;
    UserHistory &uHis = user->uHis;
    uRes.incrFortune(actualEarned);
    // update max fortune
    uHis.newFortune(uRes.fortune);
    // update max earned
    uHis.newEarned(data.earned);
    // update earned (include this week and total)
    auto before = uHis.totalEarned;
    uHis.incrEarned(data.earned);
    // fortune change event.
}



END_NAMESPACE
