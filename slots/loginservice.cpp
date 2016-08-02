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
	formatResult(pOk, sUser, ret);
	resp.setBody(ret.c_str(), ret.size());
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

    void LoginService::formatResult(bool success, SlotsUser &su, std::string &out) const {
	out.clear();
	out.append(1,'{');
	if (!success) {
	    JSON_VALUE(out, "status", "ERROR", true);
	    JSON_VALUE(out, "msg", "", false);
	    return;
	}
	JSON_VALUE(out, "status", "OK", true);	
	JSON_VALUE(out, "uid", su.uInfo.uid, true);	
	JSON_VALUE(out, "mid", su.uInfo.mid, true);
	JSON_VALUE(out, "fname", su.uInfo.fname, true);
	JSON_VALUE(out, "country", su.uInfo.country, true);
	//JSON_VALUE(out, "lname", ui.lname, true);
	JSON_VALUE(out, "level", su.uRes.level, true);
	JSON_VALUE(out, "exp", su.uRes.exp, false);
	out.append(1, '}');
    }
#undef JSON_VALUE
    
    bool LoginService::getUserInfo(CPacket &packet, SlotsUser &su) const{
	if (!packet.getParamValue("mid", su.uInfo.mid)) {
	    return false;
	}
	//get user info from db.
	SlotsDB &db = SlotsDB::getInstance();
	std::string errMsg;
	if (db.getUserInfo(su.uInfo.mid, su, errMsg)) {
	    return true;
	} 
	return false;
    }

}
