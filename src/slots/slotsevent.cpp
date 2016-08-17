#include "slotsevent.h"
BEGIN_NAMESPACE(slots)
SlotsEvent::SlotsEvent(){
}
    
SlotsEvent::~SlotsEvent(){
}

void SlotsEvent::init() {
    _slotsConfig.init();
    _gData.initFromDb();
}

void SlotsEvent::playGame(SlotsUserPtr user, SlotsEventData &data) {
    processGameDetail(user, data);
    // is free to play
    if (data.isFreeRound) {
	data.bet = 0;
    }
    // update user fortune
    processMoney(user, data);
    // update user exp&level    
    processExp(user, data);
    // update game detail
    processGameDetail(user, data);
}

void SlotsEvent::processGameDetail(SlotsUserPtr user, SlotsEventData &data){
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
}

void SlotsEvent::processExp(SlotsUserPtr user, SlotsEventData &data){
    auto &uRes = user->uRes;    
    // if zero bet then exp will not change
    if (data.bet == 0) {
	return;
    }
    auto expNeed = _slotsConfig.expNeed2LevelUp(uRes.exp);
    auto expGot = _slotsConfig.expGain(data.bet);
    // level up
    if (expNeed <= expGot ) {
	uRes.levelUp();
	// level achievement
	_amtSys.levelUP(user, data);
    }
    uRes.incrExp(expGot);
}

void SlotsEvent::processMoney(SlotsUserPtr user, SlotsEventData &data){
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
    // achievement
    _amtSys.fortuneChange(uRes.uid, before, uHis.totalEarned);
}

void SlotsEvent::fortuneChange(UserResource &uRes, int64_t money) {
    uRes.incrFortune(money);
    expChange(uRes, money);
    vipPointChange(uRes, money);
}

void SlotsEvent::expChange(UserResource &uRes, int64_t money) {
    // NOT FINISH	    
    int64_t exp = money;

    int64_t diff = _slotsConfig.expNeed2LevelUp(exp);
    uRes.incrExp(exp);	    
    if (exp >= diff){
	uRes.levelUp();
	// get reward gold number from config, then add to uRes.fortune
    }
}

void SlotsEvent::vipPointChange(UserResource &uRes, int64_t money){
    // NOT FINISH
    uRes.incrVipPoint(money);
}

void SlotsEvent::vipLevelUp(UserResource &uRes, int64_t money) {
    uRes.levelUp();
    // give gold to 
}
    
END_NAMESPACE
