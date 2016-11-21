#include <slots/services/socialityservice.h>
#include <util/stringutil.h>
#include <slots/data/slotsdatacenter.h>

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
	    ret = getAllInfo(packet, rf);
	    break;
	}
	case 2:{
	    ret = getSearchResult(packet, rf);
	    break;
	}
	case 3:{
	    ret = addFriend(packet, rf);
	    break;
	}
	case 4:{
	    ret = sendGold(packet, rf);
	    break;
	}
	case 5:{
	    ret = takeGold(packet, rf);
	    break;
	}
	case 6:{
	    ret = batchSendGold(packet, rf);
	    break;
	}
	case 7:{
	    ret = getReward(packet, rf);
	    break;
	}
	case 8:{
	    ret = removeFriend(packet, rf);
	    break;
	}
	case 9:{
	    ret = getGiftList(packet, rf);
	    break;
	}
	case 10:{
	    ret = batchGetGold(packet, rf);
	    break;
	}
	    
	}
	if (!ret) {
	    rf.formatSimpleResult(false, "");
	}
	resp.setBody(bf.GetString(), bf.GetSize());
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

    bool SocialityService::addFriend(CPacket &packet, ResultFormatter &rf) {
	std::string uid;
	GET_PARAM("uid", uid, true);
	std::string tid;
	GET_PARAM("tid", tid, true);
	SlotsDB &db = SlotsDB::getInstance();
	if (!db.makeFriend(uid, tid)) {
	    CLOG(WARNING) << "Make friend between " << uid << "&" << tid << " failed.";
	    return false;
	}
	rf.formatSimpleResult(true, "");
	return true;
    }

    bool SocialityService::sendGold(CPacket &packet, ResultFormatter &rf) {
	uint64_t uid;
	uint64_t tid;
	if (!getIntVal(packet, "uid", uid) || !getIntVal(packet, "tid", tid)) {
	    CLOG(WARNING) << "Get uid or tid from packet failed.";
	    return false;
	}
	GiftsDataPtr gData = SlotsDataCenter::instance().getGiftsData();
	if (!gData->sendGift(uid, tid)){
	    CLOG(WARNING) << "Send gift failed.";	    
	    return false;
	}
	rf.formatSimpleResult(true, "");
	return true;
    }

    bool SocialityService::takeGold(CPacket &packet, ResultFormatter &rf) {
	uint64_t uid;
	uint64_t tid;
	std::string uidStr;
	GET_PARAM("uid", uidStr, true);
	if (!cgserver::StringUtil::StrToUInt64(uidStr.c_str(), uid)
	    || !getIntVal(packet, "tid", tid))
	{
	    CLOG(WARNING) << "Get uid or tid from packet failed.";
	    return false;
	}
	SlotsDataCenter &dc = SlotsDataCenter::instance();
	GiftsDataPtr gData = dc.getGiftsData();
	if (!gData->takeGift(uid, tid)){
	    CLOG(WARNING) << "Take gift failed.";
	    return false;
	}
	// add gold to user.
        int64_t fortune;
	if (!dc.slotsUserData->addUserFortuneInCache(
                uidStr, (int64_t)gData->getGiftValue(), fortune))
        {
	    return false;
	}
	rf.formatSimpleResultWithFortune(fortune);
	return true;
    }

    bool SocialityService::getGiftList(CPacket &packet, ResultFormatter &rf) {
	uint64_t uid;
	if (!getIntVal(packet, "uid", uid)) {
	    CLOG(WARNING) << "Get uid from packet failed.";	    
	    return false;
	}
	SlotsDataCenter &dc = SlotsDataCenter::instance();
	GiftsDataPtr gData = dc.getGiftsData();
	Gifts gifts;
	gData->getGifts(uid, gifts);
	rf.formatGiftsInfo(gifts, gData->getGiftValue());
	return true;
    }

    bool SocialityService::batchSendGold(CPacket &packet, ResultFormatter &rf) {
	uint64_t uid;
	if (!getIntVal(packet, "uid", uid)) {
	    CLOG(WARNING) << "Get uid from packet failed.";	    
	    return false;
	}
	std::string tids;
	GET_PARAM("tids", tids, true);
	cgserver::strs_t receivers;
	cgserver::StringUtil::splitString(tids, ',', receivers);
	GiftsDataPtr gData = SlotsDataCenter::instance().getGiftsData();
	for (auto &val: receivers) {
	    uint64_t id;
	    if (!cgserver::StringUtil::StrToUInt64(val.c_str(), id)) {
		continue;
	    }
	    gData->sendGift(uid, id);
	}
	rf.formatSimpleResult(true, "");
	return true;	
    }

    bool SocialityService::getReward(CPacket &packet, ResultFormatter &rf) {
	std::string uid;
	GET_PARAM("uid", uid, true);
	SlotsDB &db = SlotsDB::getInstance();
	FHistory res;
	if (!db.getInviteHistory(uid, res) || !db.getReward(uid)) {
	    CLOG(WARNING) << "Operate db reward failed." ;
	    return false;
	}
	int64_t fortune;
	if(!SlotsDataCenter::instance().slotsUserData->addUserFortuneInCache(
               uid, res.rewardRemain, fortune))
        {
	    return false;
	}
	rf.formatSimpleResultWithFortune(fortune);
	return true;
    }

    bool SocialityService::removeFriend(CPacket &packet, ResultFormatter &rf) {
	std::string uid;
	std::string tid;
	GET_PARAM("uid", uid, true);
	GET_PARAM("tid", tid, true);
	SlotsDB &db = SlotsDB::getInstance();
	if (!db.removeFriend(uid, tid)){
	    return false;
	}
	rf.formatSimpleResult(true, "");
	return true;
    }

    bool SocialityService::batchGetGold(CPacket &packet, ResultFormatter &rf) {
	std::string uidStr;
	GET_PARAM("uid", uidStr, true);
	uint64_t uid;
	if (!cgserver::StringUtil::StrToUInt64(uidStr.c_str(), uid)) {
	    return false;
	}
	SlotsDataCenter &dc = SlotsDataCenter::instance();
	GiftsDataPtr gData = dc.getGiftsData();
	int64_t sum = gData->getGiftSum(uid);
	// add gold to user.
        int64_t fortune;
	if(!SlotsDataCenter::instance().slotsUserData->addUserFortuneInCache(
               uidStr, sum, fortune))
        {
	    return false;
	}
	rf.formatSimpleResultWithFortune(fortune);
	return true;
    }

    bool SocialityService::getAllInfo(CPacket &packet, ResultFormatter &rf) {
	/*get all fiend info. Include friendlist, f_history info*/
	std::string uid;
	GET_PARAM("uid", uid, true);
	SlotsDB &db = SlotsDB::getInstance();
	FriendsList friends;
	if (!db.getFriendsList(uid, 0, 10, friends)) {
	    CLOG(WARNING) << "Get friend list of user[" << uid << "] from db failed";
	    return false;
	}
	FHistory history;
	if (!db.getInviteHistory(uid, history)) {
	    CLOG(WARNING) << "Get friend history of user[" << uid << "] from db failed";
	    return false;
	}
	rf.formatFullFriendInfo(friends, history);
	return true;
    }    
}
