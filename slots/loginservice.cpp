#include "loginservice.h"

namespace slots{
    LoginService::LoginService(){
    }
    
    LoginService::~LoginService(){
    }

    bool LoginService::doJob(CPacket &packet, CResponse &resp){
	bool pOk = false;
	cgserver::LuaToolPtr lua = cgserver::LuaToolFactory::getInstance().borrowTool();
	// borrow lua tool.
	if (lua.get() == NULL) {
	    resp.setBody(common::ERR_LOAD_LUA.c_str());
	    return false;
	}
	do {
	    UserInfo userInfo;
	    if (!getUserInfo(packet, userInfo)){
		resp.setBody("Get user info failed.");
		break;
	    }
	    if (!lua->exeFunc("entrance", "123", 1)) {
		resp.setBody(common::ERR_CALL_LUA.c_str());	    
		break;
	    }
	    std::string ret;
	    lua->getValue(ret);
	    lua->debug();
	    resp.setBody(ret.c_str());
	    pOk = true;
	}while(0);
	// lua tool must return!
	cgserver::LuaToolFactory::getInstance().returnTool(lua);
	return pOk;
    }

    bool LoginService::getUserInfo(CPacket &packet, UserInfo &uinfo) {
	if (!packet.getParamValue("mid", uinfo.mid)) {
	    return false;
	}
	//get user info from db.
	return true;
    }

}
