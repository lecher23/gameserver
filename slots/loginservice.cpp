#include "loginservice.h"

namespace slots{
    LoginService::LoginService(){
    }
    
    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
	//cgserver::LuaToolPtr lua = cgserver::LuaToolFactory::getInstance().borrowTool();
	// borrow lua tool.
	// if (lua.get() == NULL) {
	//     resp.setBody(common::ERR_LOAD_LUA.c_str());
	//     return false;
	// }
	SlotsUser sUser;	
	do {
	    if (!getUserInfo(packet, sUser)){
		break;
	    }
	    pOk = true;	    
	    // if (!lua->exeFunc("entrance", "123", 1)) {
	    // 	resp.setBody(common::ERR_CALL_LUA.c_str());	    
	    // 	break;
	    // }
	    // std::string ret;
	    // lua->getValue(ret);
	    // lua->debug();
	}while(0);
	ResultFormatter rf;
	SBuf buf;
	if (pOk) {
	    rf.formatResult(sUser, buf);
	} else {
	    rf.formatSimpleResult(buf, pOk, "");
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
