#include "socialityservice.h"
#include "../util/stringutil.h"

using namespace cgserver;
namespace slots{
    SocialityService::SocialityService(){
    }
    
    SocialityService::~SocialityService(){
    }
    
    bool SocialityService::doJob(CPacket &packet, CResponse &resp) {
	std::string rTypeStr;
	GET_PARAM("type", rTypeStr, true);
	uint32_t rType;
	if (!StringUtil::StrToUInt32(rTypeStr.c_str(), rType)) {
	    CLOG(WARNING) << "Invalid type:" << rTypeStr;
	    return false;
	}
	
	SBuf bf;
	ResultFormatter rf(bf);
	bool ret = false;
	switch(rType){
	case 0:{
	    ret = getFriendsList(packet, rf);
	    break;
	}
	case 1:{
	    // get all information to be shown in friends dialog
	    // ret = getAllInfo(packet, rf);
	    break;
	}
	case 2:{
	    ret = getSearchResult(packet, rf);
	    break;
	}
	}
	resp.setBody(bf.GetString());
	return false;
    }

    bool SocialityService::getFriendsList(CPacket &packet, ResultFormatter &rf) {
	std::string uid;
	GET_PARAM("uid", uid, true);
	uint32_t page, pageSize;
	if (!getIntVal(packet, "page", page)) {
	    page = 0;
	}
	if (!getIntVal(packet, "pagesize", pageSize)) {
	    pageSize = 10;
	}
	pageSize = pageSize > MAX_PAGE_SIZE ? MAX_PAGE_SIZE : pageSize;
	SlotsDB &db = SlotsDB::getInstance();
	FriendsList friends;
	if (!db.getFriendsList(uid, page, pageSize, friends)) {
	    CLOG(WARNING) << "Get friend list fo user[" << uid << "] from db failed";
	    return false;
	}
	rf.formatFriendsInfo(friends);
	return true;
    }

    bool SocialityService::getSearchResult(CPacket &packet, ResultFormatter &rf) {
	std::string kwStr;
	GET_PARAM("kw", kwStr, false);
	std::string sField = "fname";
	if (kwStr.empty()){
	    GET_PARAM("tid", kwStr, true);
	    sField = "user_resource.uid";
	}
	
	uint32_t page, pageSize;
	if (!getIntVal(packet, "page", page)) {
	    page = 0;
	}
	if (!getIntVal(packet, "pagesize", pageSize)) {
	    pageSize = 10;
	}
	pageSize = pageSize > MAX_PAGE_SIZE ? MAX_PAGE_SIZE : pageSize;
	
	SlotsDB &db = SlotsDB::getInstance();
	SlotsUsers userList;
	if (!db.searchUser(sField, kwStr, page, pageSize, userList)) {
	    CLOG(WARNING) << "Search friend with keyword[" << kwStr << "] from db failed";
	    return false;
	}
	rf.formatFriendsInfo(userList);
	return true;
    }
}
