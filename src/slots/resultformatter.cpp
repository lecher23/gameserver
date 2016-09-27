#include "resultformatter.h"
BEGIN_NAMESPACE(slots)
ResultFormatter::ResultFormatter(SBuf &buffer):_writer(buffer){
}
    
ResultFormatter::~ResultFormatter(){
}

void ResultFormatter::formatResult(const UserMails &uMails)  {
    _writer.StartObject();
    formatStatus(true);
    formatMailsInfo(uMails);
    _writer.EndObject();
}

void ResultFormatter::formatResult(const SlotsUser &su)  {
    _writer.StartObject();
    formatStatus(true);

    _writer.Key("user_info");
    formatUserInfo(su.uInfo);

    _writer.Key("user_resource");
    formatUserResource(su.uRes);

    _writer.Key("login_info");
    formatLoginInfo(su);

    _writer.EndObject();
}

void ResultFormatter::formatResult(const SlotsUser &su, const Achievements &cj) {
    _writer.StartObject();
    formatStatus(true);
    _writer.Key("user_info");
    formatUserInfo(su.uInfo);

    _writer.Key("user_resource");
    formatUserResource(su.uRes);

    _writer.Key("user_achievement");
    formatUserAchievement(cj);

    _writer.Key("login_info");
    formatLoginInfo(su);

    _writer.EndObject();
}
    
void ResultFormatter::formatSimpleResult(bool success, const std::string &err)
{
    _writer.StartObject();
    formatStatus(success);
    _writer.Key("msg");
    _writer.String(err.c_str());
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, int64_t value)
{
    _writer.StartObject();
    formatStatus(success);
    _writer.Key(key.c_str());
    _writer.Int64(value);
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, int32_t value)
{
    _writer.StartObject();
    formatStatus(success);
    _writer.Key(key.c_str());
    _writer.Int(value);
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, const std::string &value)
{
    _writer.StartObject();
    formatStatus(success);
    _writer.Key(key.c_str());
    _writer.String(value.c_str());
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, bool value)
{
    _writer.StartObject();
    formatStatus(success);
    _writer.Key(key.c_str());
    _writer.Bool(value);
    _writer.EndObject();
}

void ResultFormatter::formatGameResult(
    const UserResource &sr, int64_t earned, const std::string &detail)
{
    _writer.StartObject();
    formatStatus(true);
    _writer.Key("uid");
    _writer.String(sr.uid.c_str());
    _writer.Key("user_resource");
    formatUserResource(sr);
    _writer.Key("earned");
    _writer.Int64(earned);
    _writer.Key("detail");
    _writer.String(detail.c_str());
    _writer.EndObject();
}

void ResultFormatter::formatFriendsInfo(const FriendsList &friends) {
    _writer.StartObject();
    formatStatus(true);
    formatFriendList(friends);
    _writer.EndObject();
}

void ResultFormatter::formatFullFriendInfo(
    const FriendsList &friends, const FHistory &history)
{
    _writer.StartObject();
    formatStatus(true);
    formatFriendList(friends);
    _writer.Key("invite_count");
    _writer.Int(history.inviteCount);
    _writer.Key("total_reward");	
    _writer.Int64(history.totalReward);
    _writer.Key("cached_reward");	
    _writer.Int64(history.rewardRemain);
    _writer.EndObject();
}    

void ResultFormatter::formatGiftsInfo(const Gifts &gifts, int64_t giftsVal) {
    _writer.StartObject();
    formatStatus(true);
    _writer.Key("gifts");
    _writer.StartArray();
    for (auto itr = gifts.begin(); itr != gifts.end(); ++itr) {
	_writer.StartObject();
	_writer.Key("uid");
	_writer.Int64((int64_t)(*itr)->master);
	_writer.Key("received");
	_writer.Bool((int64_t)(*itr)->received);
	_writer.Key("timestamp");
	_writer.Int64((*itr)->timestamp);
	_writer.EndObject();
    }
    _writer.EndArray();
    _writer.Key("value");
    _writer.Int64(giftsVal);
    _writer.EndObject();
}

// [start, end)
void ResultFormatter::formatRankResult(
    const LeaderBoardRank &rank, int64_t uid, uint32_t start, uint32_t end)
{
    _writer.StartObject();
    formatStatus(true);
    _writer.Key("data");
    _writer.StartArray();
    uint32_t cur = 0;
    int32_t rankValue = 0;
    for (auto &itr: rank.data) {
	if (cur >= start && cur < end) {
	    _writer.StartObject();
	    _writer.Key("uid");
	    _writer.Int64(itr->uid);
	    _writer.Key("name");
	    _writer.String(itr->name.c_str());
	    _writer.Key("country");
	    _writer.Int(itr->country);
	    _writer.Key("value");
	    _writer.Int64(itr->value);
	    _writer.EndObject();
	}
	++cur;
	if (itr->uid == uid) {
	    rankValue = cur;
	}
    }
    _writer.EndArray();
    _writer.Key("myrank");
    _writer.Int(rankValue);
    _writer.EndObject();
}

void ResultFormatter::formatStatus(bool bOk) {
    _writer.Key("status");
    _writer.String( bOk ? "OK" : "ERROR" );	
}

/*"mails":[{..mail info },{}..]*/    
void ResultFormatter::formatMailsInfo(const UserMails &uMails)
{
    _writer.Key("mails");
    _writer.StartArray();
    for (auto itr = uMails.begin(); itr != uMails.end(); ++itr) {
	formatMail(*(*itr));
    }
    _writer.EndArray();
}

/*{"id":xxx,...}*/    
void ResultFormatter::formatMail(const UserMail &uMail) {
    auto &mi = uMail.mailInfo;
    _writer.StartObject();
    _writer.Key("id");
    _writer.String(mi.mailId.c_str());
    _writer.Key("title");
    _writer.String(mi.title.c_str());
    _writer.Key("content");
    _writer.String(mi.content.c_str());
    // _writer.Key("attachment");
    // _writer.String(uMail.mailInfo.attachment);
    AttachmentPtr ap;
    bool exist;
    ap = SlotsDataCenter::instance().getAttach(mi.attachment, exist);
    _writer.Key("type");
    _writer.Int(exist ? ap->type : 0);
    if (exist) {
	_writer.Key("value");
	_writer.Int64(ap->value);
    }
    _writer.Key("create_time");
    _writer.String(uMail.mailInfo.createTime.c_str());
    _writer.Key("keey_days");
    _writer.Int(uMail.mailInfo.keepDays);
    _writer.Key("is_read");
    _writer.Bool(uMail.bRead);
    _writer.Key("is_get");
    _writer.Bool(uMail.bGet);
    _writer.Key("is_del");
    _writer.Bool(uMail.bDel);
    _writer.EndObject();	
}

/* 
   {"uid": "uid",...}
*/
void ResultFormatter::formatUserInfo(const UserInfo &uInfo) {
    _writer.StartObject();
    _writer.Key("uid");
    _writer.String(uInfo.uid.c_str());
    _writer.Key("mid");
    _writer.String(uInfo.mid.c_str());
    _writer.Key("fname");
    _writer.String(uInfo.fname.c_str());
    _writer.Key("avatar");
    _writer.String(uInfo.avatar.c_str());
    _writer.Key("male");
    _writer.String(uInfo.male.c_str());
    _writer.Key("country");
    _writer.String(uInfo.country.c_str());
    _writer.EndObject();	
}

/* 
   {"uid": "uid",...}
*/
void ResultFormatter::formatUserResource(const UserResource &uRes) {
    _writer.StartObject();	
    _writer.Key("uid");
    _writer.String(uRes.uid.c_str());
    _writer.Key("level");
    _writer.Uint(uRes.level);
    _writer.Key("exp");
    _writer.Uint64(uRes.exp);
    _writer.Key("fortune");
    _writer.Uint64(uRes.fortune);
    _writer.Key("vip_level");
    _writer.Uint(uRes.vipLevel);
    // _writer.Key("vip_point");
    // _writer.String(uRes.vipPoint);
    _writer.EndObject();
}

void ResultFormatter::formatUserAchievement(const Achievements &cj) {
    _writer.StartArray();
    for (auto itr = cj.begin(); itr != cj.end(); ++itr) {
	auto &iCj = **itr;
	_writer.StartObject();
	_writer.Key("id");
	_writer.String(iCj.aid.c_str());
	_writer.Key("progress");
	_writer.Int64(iCj.progress);
	_writer.Key("gained");
	_writer.Bool(iCj.isGain);
	_writer.Key("recved");
	_writer.Bool(iCj.isRecvReward);
	_writer.Key("time");
	_writer.Int64(iCj.time);
	_writer.EndObject();
    }
    _writer.EndArray();
}

void ResultFormatter::formatFriendList(const FriendsList &friends) {
    _writer.Key("friends");
    _writer.StartArray();
    for (auto itr = friends.begin(); itr != friends.end(); ++itr) {
	_writer.StartObject();
	_writer.Key("uid");
	_writer.String((*itr)->uInfo.uid.c_str());
	_writer.Key("avatar");
	_writer.String((*itr)->uInfo.uid.c_str());
	_writer.Key("name");
	_writer.String((*itr)->uInfo.fname.c_str());
	_writer.Key("golds");
	_writer.Int64((*itr)->uRes.fortune);
	_writer.EndObject();	    
    }
    _writer.EndArray();
}

void ResultFormatter::formatLoginInfo(const SlotsUser &su) {
    _writer.StartObject();
    _writer.Key("days");
    _writer.Int(su.gDetail.loginDays);
    _writer.Key("recv_reward");
    _writer.Bool(su.loginReward.recved);
    _writer.Key("level_bonus");
    _writer.Int64(su.loginReward.specialReward);
    _writer.Key("runner_index");
    _writer.Int64(su.loginReward.runnerIdx);
    _writer.Key("day_bonus");
    _writer.Int64(su.loginReward.daysReward);
    _writer.EndObject();
}

END_NAMESPACE
