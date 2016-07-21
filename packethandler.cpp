#include "packethandler.h"
#include <string.h>
#include <iostream>

namespace cglogic{
    using namespace cgserver;
    const std::string PacketHandler::acceptURI = "/getUserSetting";
    const std::string PacketHandler::User_ID = "user_id";
    void PacketHandler::init(){
	cfg["010"] = "YES";
    }

    bool PacketHandler::process(HTTPPacket &packet, HttpResponsePacket &resp) const{
	resp.setStatus(true);	    	
	if (packet.getMethod() != HTTPPacket::HM_POST) {
	    resp.setBody("Only support POST method.");
	    resp.setStatus(false);
	    return false;
	}
	if (packet.getPath() != acceptURI) {
	    resp.setBody("Invalid PATH.");
	    resp.setStatus(false);	    
	    return false;	    
	}
	bool exist;
	const std::string &uid = packet.getParamValue(User_ID, exist);
	if (!exist)
	    resp.setBody("NO");
	else {
	    std::map<std::string, std::string>::const_iterator citr = cfg.find(uid);
	    if (citr != cfg.end())
		resp.setBody((citr->second).c_str());
	    else
		resp.setBody("NO");
	}
	return true;
    }
}
