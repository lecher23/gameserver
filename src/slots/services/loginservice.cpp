#include "loginservice.h"
#include <slots/processors/loginprocessor.h>
#include <slots/processors/achievementprocessor.h>

namespace slots{
    LoginService::LoginService(){
    }

    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
	SlotsUserPtr sUser;
	Achievements cj;
	do {
	    if (!getUserInfo(packet, sUser, cj)){
		break;
	    }
	    pOk = true;
	}while(0);

        GameContext context;
        context.user = sUser;

        LoginProcessor lp;
        lp.process(context);
        AchievementProcessor ap;
        ap.process(context);

	SBuf buf;
	ResultFormatter rf(buf);
	if (pOk) {
	    rf.formatResult(*sUser, cj);
	} else {
	    rf.formatSimpleResult(pOk, "");
	}

	resp.setBody(buf.GetString());
	return pOk;
    }

    bool LoginService::getUserInfo(
        CPacket &packet, SlotsUserPtr &su, Achievements &cj) const
    {
	std::string mid;
	if (!packet.getParamValue("mid", mid)) {
	    return false;
	}
	auto &slotsUserData = SlotsDataCenter::instance().slotsUserData;
	//get user info from db.
	SlotsDB &db = SlotsDB::getInstance();
	if (slotsUserData->getByMid(mid, su)) {
	    return db.getUserAchievement(su->uInfo.uid, cj);
	}
	return false;
    }

}
