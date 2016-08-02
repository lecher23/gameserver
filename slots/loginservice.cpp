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
	UserInfo userInfo;	
	do {
	    if (!getUserInfo(packet, userInfo)){
		resp.setBody("Get user info failed.");
		break;
	    }
	    // if (!lua->exeFunc("entrance", "123", 1)) {
	    // 	resp.setBody(common::ERR_CALL_LUA.c_str());	    
	    // 	break;
	    // }
	    // std::string ret;
	    // lua->getValue(ret);
	    // lua->debug();
	    pOk = true;
	}while(0);
	std::string ret;
	formatResult(pOk, userInfo, ret);
	resp.setBody(ret.c_str());
	// lua tool must return!
	//cgserver::LuaToolFactory::getInstance().returnTool(lua);
	return pOk;
    }

#define JSON_VALUE(str, key, val, seq)		\
    str.append(1,'"');				\
    str.append(key);				\
    str.append("\":\"");			\
    str.append(val);				\
    str.append(1, '"');				\
    if (seq) {					\
	str.append(1, ',');			\
    }

    void LoginService::formatResult(bool success, UserInfo &ui, std::string &out) const {
	out.clear();
	out.append(1,'{');
	if (!success) {
	    JSON_VALUE(out, "status", "ERROR", true);
	    JSON_VALUE(out, "msg", "", false);
	    return;
	}
	JSON_VALUE(out, "status", "OK", true);	
	JSON_VALUE(out, "uid", ui.uid, true);	
	JSON_VALUE(out, "mid", ui.mid, true);
	if (ui.fname.empty()) {
	    JSON_VALUE(out, "new-user", "1", true);
	}else {
	    JSON_VALUE(out, "new-user", "0", true);
	}
	JSON_VALUE(out, "fname", ui.fname, true);
	JSON_VALUE(out, "lname", ui.lname, true);
	JSON_VALUE(out, "level", ui.level, true);
	JSON_VALUE(out, "exp", ui.exp, true);
	out.append(1, '}');
    }
#undef JSON_VALUE
    
    bool LoginService::getUserInfo(CPacket &packet, UserInfo &uinfo) const{
	if (!packet.getParamValue("mid", uinfo.mid)) {
	    return false;
	}
	//get user info from db.
	SlotsDB &db = SlotsDB::getInstance();
	std::string errMsg;
	if (db.getUserInfo(uinfo.mid, uinfo, errMsg)) {
	    return true;
	} 
	return false;
    }

}
