#include "loginservice.h"

namespace slots{
    LoginService::LoginService(){
    }

    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
	SlotsUserPtr sUser;
	Achievements cj;
        GameContext context;
	do {
	    if (!getUserInfo(packet, sUser, cj)){
		break;
	    }
            context.user = sUser;
            _lProcessor.process(context);
            _aProcessor.process(context);
            pOk = true;
	}while(0);

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
