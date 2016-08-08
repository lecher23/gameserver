#include "packethandler.h"
#include <string.h>
#include <iostream>
#include "../util/stringutil.h"
#include "../util/config.h"

namespace cglogic{
    using namespace cgserver;

    const std::string PacketHandler::AppName = "app1";
    const std::string PacketHandler::AcceptPrefix = "prefix";
    const std::string PacketHandler::TargetParam = "target_param";
    const std::string PacketHandler::RefreshTrigger = "refresh_trigger";
    const std::string PacketHandler::DefaultCode = "default_code";
    const std::string PacketHandler::SpecialSetting = "special_setting";

    PacketHandler::~PacketHandler(){}
    
    void PacketHandler::init(){
	CLOG(INFO) << "**Init packet handler**" ;
	Config &cfg = Config::getInstance();
	_acceptPrefix = cfg.getConfigValue(AppName, AcceptPrefix);
	_triggerPath = cfg.getConfigValue(AppName, RefreshTrigger);
	_targetParam = cfg.getConfigValue(AppName, TargetParam);
	_defaultResponse = cfg.getConfigValue(AppName, DefaultCode);
	std::string sc = cfg.getConfigValue(AppName, SpecialSetting);
	cgserver::strs_t pairs;
	cgserver::strs_t kv;
	cgserver::StringUtil::splitString(sc, ';', pairs);
	for (strs_t::iterator it = pairs.begin(); it != pairs.end(); ++it) {
	    cgserver::StringUtil::splitString(*it, ':', kv);
	    if (kv.size() != 2) {
		CLOG(INFO) << "Invalid setting [" << *it << "], ignore." ;
		continue;
	    }
	    _cfg[kv[0]] = kv[1];
	    kv.clear();
	}
	CLOG(INFO) << "**Init packet handler success**" ;	
    }

    bool PacketHandler::process(HTTPPacket &packet, HttpResponsePacket &resp) {
	resp.setStatus(true);
	if (packet.getMethod() != HTTPPacket::HM_POST) {
	    resp.setBody("Only support POST method.\n");
	    resp.setStatus(false);
	    return false;
	}
	if (packet.getPath() == _triggerPath) {
	    Config::getInstance().refreshConfig();
	    init();// here we can use singleton to storage config detail
	    resp.setBody("Refresh config done.\n");
	    return true;
	}
	if (packet.getPath() != _acceptPrefix) {
	    std::string msg = "Invalid PATH:";
	    msg.append(packet.getPath());
	    msg.append(1, '\n');
	    resp.setBody(msg.c_str());
	    resp.setStatus(false);	    
	    return false;	    
	}
	bool exist;
	const std::string &uid = packet.getParamValue(_targetParam, exist);
	if (!exist)
	    resp.setBody(_defaultResponse.c_str());
	else {
	    std::map<std::string, std::string>::const_iterator it = _cfg.find(uid);
	    if (it != _cfg.end()){
		resp.setBody((it->second).c_str());
	    }else
		resp.setBody(_defaultResponse.c_str());
	}
	return true;
    }
}
