#include "loginservice.h"

namespace slots{
    LoginService::LoginService(){
    }
    
    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
	SlotsUser sUser;	
	do {
	    if (!getUserInfo(packet, sUser)){
		break;
	    }
	    pOk = true;	    
	}while(0);
	
	SBuf buf;
	ResultFormatter rf(buf);
	if (pOk) {
	    rf.formatResult(sUser);
	} else {
	    rf.formatSimpleResult(pOk, "");
	}
	
	//formatResult(pOk, sUser, ret);
	resp.setBody(buf.GetString());
	// lua tool must return!
	//cgserver::LuaToolFactory::getInstance().returnTool(lua);
	return pOk;
    }
    
    bool LoginService::getUserInfo(CPacket &packet, SlotsUser &su) const{
	if (!packet.getParamValue("mid", su.uInfo.mid)) {
	    return false;
	}
	//get user info from db.
	SlotsDB &db = SlotsDB::getInstance();
	if (db.getUserInfoByMachineId(su.uInfo.mid, su)) {
	    return true;
	} 
	return false;
    }

}
