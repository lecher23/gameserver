#include "packethandler.h"
#include <string.h>
#include <iostream>

namespace cglogic{
    using namespace cgserver;
    void PacketHandler::init(){
	cfg["010"] = "YES";
    }

    bool PacketHandler::process(HTTPPacket &packet, HttpResponsePacket &resp) {
	resp.setStatus(true);	    	
	if (packet.getMethod() != HTTPPacket::HM_POST) {
	    resp.setBody("Only support POST method.");
	    resp.setStatus(false);
	    return false;
	}
	if (!parseURI(packet.getURI())){
	    resp.setBody("Invalid URI.");
	    resp.setStatus(false);	    
	    return false;
	}
	if (_uri != "/getUserSetting") {
	    resp.setBody("Invalid PATH.");
	    resp.setStatus(false);	    
	    return false;	    
	}
	KVMapItr itr = params.find("user_id");
	if (itr == params.end())
	    resp.setBody("NO");
	else {
	    KVMapItr citr = cfg.find(itr->second);
	    if (citr != cfg.end())
		resp.setBody((citr->second).c_str());
	    else
		resp.setBody("NO");
	}
	return true;
    }

    bool PacketHandler::parseURI(const char *uri) {
	int uriLen = strlen(uri);
	int qFlag = findChar(uri, '?');
	if (qFlag < 0)
	    return true;
	_uri.assign(uri, qFlag);
	if(!parseParam(uri + qFlag + 1))
	    return false;
	return true;
    }

    int PacketHandler::findChar(const char *dest, char tar) {
	int ret = 0;
	while(*dest != '\0') {
	    if (*dest == tar) return ret;
	    ++dest;
	    ++ret;
	}
	return -1;
    }

    // x=1&y=3
    bool PacketHandler::parseParam(const char *paramStr){
	int prev = 0;
	int pos;
	while((pos = findChar(paramStr, '&')) >= 0){
	    parseKV(paramStr, pos);
	    paramStr += pos + 1;
	}
	if (*paramStr != '\0') {
	    parseKV(paramStr, strlen(paramStr));
	}
	return true;
    }

    //x=1, 3
    void PacketHandler::parseKV(const char *begin, int kvLen) {
	if (kvLen <= 0)
	    return;
	int pos = findChar(begin, '=');
	if (pos == kvLen -1)
	    params[std::string(begin, pos)] = "";
	else if (pos < 0)
	    params[std::string(begin, kvLen)] = "";
	else
	    params[std::string(begin, pos)] = std::string(begin + pos + 1, kvLen - pos -1);
    }

    void PacketHandler::debug(){
	std::map<std::string, std::string>::iterator itr = params.begin();
	std::cout << "Params in uri." << std::endl;
	while(itr != params.end()) {
	    std::cout << itr->first << ":" << itr->second << std::endl;
	    ++itr;
	}
	std::cout << _uri << std::endl;
    }
}
