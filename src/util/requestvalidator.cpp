#include "requestvalidator.h"
#include <iostream>
#include "md5tool.h"
#include "common_define.h"
#include "timeutil.h"
#include "stringutil.h"

namespace cgserver{
    RequestValidator::RequestValidator(){
    }
    
    RequestValidator::~RequestValidator(){
    }

    /* required param in uri:sign & ts*/
    bool RequestValidator::validate(HTTPPacket &packet, const std::string &secret) {
	const HTTPPacket::KVMap &params = packet.getParams();	
	// get timestamp, and makesure it not out of range
	// seed: timestamp(ms) + random(1000, 9999)
	std::string seed;
	if (!packet.getParamValue("ts", seed)) {
	    return false;
	}
	seed.resize(seed.size() - 4);
	int64_t ts;
	if (!StringUtil::StrToInt64(seed.c_str(), ts)) {
	    return false;
	}
	int64_t current = CTimeUtil::getCurrentTimeInMillSeconds();
	// timeout 5 second
	if (current < ts || current - ts > 5000) {
	    return false;
	}
	// get signature value
	std::string signature;
	if (!packet.getParamValue("sign", signature)) {
	    return false;
	}
	// get str to sign
	std::string str2sign;
	for (auto itr = params.begin(); itr != params.end(); ++itr) {
	    if (itr->first == "sign") {
		continue;
	    }
	    str2sign += itr->first;
	    str2sign += "=";
	    str2sign += itr->second;	    
	    str2sign += "&";
	}
	str2sign += secret;
	std::cout << str2sign << std::endl;

	//caculate md5
	Md5Tool md5;
	md5.update(str2sign);
	return signature == md5.toString();
    }
}
