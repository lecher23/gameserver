#include "slotsevent.h"
namespace slots{
    SlotsEvent::SlotsEvent(){
    }
    
    SlotsEvent::~SlotsEvent(){
    }

    void SlotsEvent::init() {
	_slotsConfig.init();
    }

    void SlotsEvent::processData(SlotsUserPtr user, SlotsEventData &data){
	int64_t actualEarned = data.earned - data.bet;
	if (actualEarned == 0) {
	    return;
	}
	UserResource &uRes = user->uRes;
	UserHistory &uHis = user->uHis;
	uRes.incrFortune(actualEarned);
	uHis.newFortune(uRes.fortune);
	uHis.newEarned(data.earned);
	uHis.incrEarned(data.earned);
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
    
}
