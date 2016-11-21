#include "loginservice.h"

namespace slots{
    LoginService::LoginService(){
    }

    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
        GameContext context;
	do {
	    if (!getUserInfo(packet, context)){
		break;
	    }
            _lProcessor.process(context);
            _aProcessor.process(context);
            pOk = true;
	}while(0);

	SBuf buf;
	ResultFormatter rf(buf);
	if (pOk) {
	    rf.formatResultWithCj(context);
	} else {
	    rf.formatSimpleResult(pOk, "");
	}

	resp.setBody(buf.GetString(), buf.GetSize());
	return pOk;
    }

    bool LoginService::getUserInfo(
        CPacket &packet, GameContext &user) const
    {
	std::string mid;
	if (!packet.getParamValue(slotconstants::sMachineID, mid)) {
            CLOG(WARNING) << "get mid failed.";
	    return false;
	}
	return SlotsDataCenter::instance().slotsUserData->getUserByMid(mid, user);
    }

}
