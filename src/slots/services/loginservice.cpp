#include "loginservice.h"

namespace slots{
    LoginService::LoginService(){
    }

    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
	SlotsUserPtr sUser;
        GameContext context;
	do {
	    if (!getUserInfo(packet, sUser)){
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
	    rf.formatResultWithCj(context);
	} else {
	    rf.formatSimpleResult(pOk, "");
	}

	resp.setBody(buf.GetString());
	return pOk;
    }

    bool LoginService::getUserInfo(
        CPacket &packet, SlotsUserPtr &su) const
    {
	std::string mid;
	if (!packet.getParamValue(slotconstants::sMachineID, mid)) {
	    return false;
	}
	return SlotsDataCenter::instance().slotsUserData->getByMid(mid, su);
    }

}
