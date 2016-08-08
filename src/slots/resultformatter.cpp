#include "resultformatter.h"
namespace slots{
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
	_writer.StartObject();
	_writer.Key("id");
	_writer.String(uMail.mailInfo.mailId.c_str());
	_writer.Key("title");
	_writer.String(uMail.mailInfo.title.c_str());
	_writer.Key("content");
	_writer.String(uMail.mailInfo.content.c_str());
	// _writer.Key("attachment");
	// _writer.String(uMail.mailInfo.attachment);
	_writer.Key("create_time");
	_writer.String(uMail.mailInfo.createTime.c_str());
	_writer.Key("keey_days");
	_writer.String(uMail.mailInfo.keepDays.c_str());
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
}
