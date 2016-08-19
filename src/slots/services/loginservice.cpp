#include "loginservice.h"

namespace slots{
    LoginService::LoginService(){
    }
    
    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
	SlotsUser sUser;	
	Achievements cj;
	do {
	    if (!getUserInfo(packet, sUser, cj)){
		break;
	    }
	    pOk = true;
	}while(0);
	
	SBuf buf;
	ResultFormatter rf(buf);
	if (pOk) {
	    rf.formatResult(sUser, cj);
	} else {
	    rf.formatSimpleResult(pOk, "");
	}
	
	resp.setBody(buf.GetString());
	return pOk;
    }
    
    bool LoginService::getUserInfo(CPacket &packet, SlotsUser &su, Achievements &cj) const{
	if (!packet.getParamValue("mid", su.uInfo.mid)) {
	    return false;
	}
	//get user info from db.
	SlotsDB &db = SlotsDB::getInstance();
	if (db.getUserInfoByMachineId(su.uInfo.mid, su)) {
	    return db.getUserAchievement(su.uInfo.uid, cj);
	} 
	return false;
    }

}
