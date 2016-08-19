#include "updateservice.h"
namespace slots{
    UpdateService::UpdateService(){
    }
    
    UpdateService::~UpdateService(){
    }
    
    bool UpdateService::doJob(CPacket &packet, CResponse &resp){
	std::string type;
	std::string errMsg;
	std::string out;
	if (!packet.getParamValue("type", type)) {
	    CLOG(ERROR) << "Param type required.";
	    errMsg = "Code 003";
	    formatResult(false, errMsg, out);
	    resp.setBody(out.c_str(), out.size());
	    return true;
	}
	if (type == "0" ) {
	    formatResult(updateUserInfo(packet), errMsg, out);
	    resp.setBody(out.c_str(), out.size());	    
	}
	return true;
    }

    bool UpdateService::updateUserInfo(CPacket &packet) {
	UserInfo ui;
	GET_PARAM("uid", ui.uid, true);
	GET_PARAM("male", ui.male, false);
	GET_PARAM("country", ui.country, false);
	GET_PARAM("fname", ui.fname, false);
	GET_PARAM("avatar", ui.avatar, false);
	SlotsDB &db = SlotsDB::getInstance();
	if (!db.updateUserInfo(ui)) {
	    CLOG(WARNING) << "Update user[" << ui.uid << "] info failed.";
	    return false;
	}
	return true;
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

    void UpdateService::formatResult(bool status, const std::string &msg, std::string &out)
    {
	out.clear();
	out.append(1, '{');
	JSON_VALUE(out, "status", status ? "OK":"ERROR", true);
	JSON_VALUE(out, "msg", msg, false);
	out.append(1, '}');
    }
#undef JSON_VALUE    
}