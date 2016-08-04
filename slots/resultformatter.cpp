#include "resultformatter.h"
namespace slots{
    ResultFormatter::ResultFormatter(){

    }
    
    ResultFormatter::~ResultFormatter(){
    }

    void ResultFormatter::formatResult(const UserMails &uMails, SBuf &buffer)  {
	JsonWriter writer(buffer);	
	writer.StartObject();
	formatStatus(writer, true);
	formatMailsInfo(writer, uMails);
	writer.EndObject();
    }

    void ResultFormatter::formatResult(const SlotsUser &su, SBuf &buffer)  {
	JsonWriter writer(buffer);	
	writer.StartObject();
	formatStatus(writer, true);
	
	writer.Key("user_info");
	writer.StartObject();
	formatUserInfo(writer, su.uInfo);
	writer.EndObject();

	writer.Key("user_resource");	
	writer.StartObject();
	formatUserResource(writer, su.uRes);	
	writer.EndObject();
	
	writer.EndObject();
    }
    
    void ResultFormatter::formatSimpleResult(SBuf &buffer, bool success, const std::string &err)
    {
	JsonWriter writer(buffer);	
	writer.StartObject();
	formatStatus(writer, success);
	writer.Key("msg");
	writer.String(err.c_str());
	writer.EndObject();
    }

    void ResultFormatter::formatStatus(JsonWriter &writer, bool bOk) {
	writer.Key("status");
	writer.String( bOk ? "OK" : "ERROR" );	
    }

    void ResultFormatter::formatMailsInfo(
	JsonWriter &writer, const UserMails &uMails)
    {
	/*"mails":[{..mail info },{}..]*/
	writer.Key("mails");
	writer.StartArray();
	for (auto itr = uMails.begin(); itr != uMails.end(); ++itr) {
	    formatMail(writer, *(*itr));
	}
	writer.EndArray();
    }

    void ResultFormatter::formatMail(JsonWriter &writer, const UserMail &uMail) {
	/*{"id":xxx,...}*/
	writer.StartObject();
	writer.Key("id");
	writer.String(uMail.mailInfo.mailId.c_str());
	writer.Key("title");
	writer.String(uMail.mailInfo.title.c_str());
	writer.Key("content");
	writer.String(uMail.mailInfo.content.c_str());
	// writer.Key("attachment");
	// writer.String(uMail.mailInfo.attachment);
	writer.Key("create_time");
	writer.String(uMail.mailInfo.createTime.c_str());
	writer.Key("keey_days");
	writer.String(uMail.mailInfo.keepDays.c_str());
	writer.Key("is_read");
	writer.Bool(uMail.bRead);
	writer.Key("is_get");
	writer.Bool(uMail.bGet);
	writer.Key("is_del");
	writer.Bool(uMail.bDel);
	writer.EndObject();	
    }

    /* without '{', '}' at begin and end
      "uid": "uid",...
     */
    void ResultFormatter::formatUserInfo(JsonWriter &writer, const UserInfo &uInfo) {
	writer.Key("uid");
	writer.String(uInfo.uid.c_str());
	writer.Key("mid");
	writer.String(uInfo.mid.c_str());
	writer.Key("fname");
	writer.String(uInfo.fname.c_str());
	writer.Key("avatar");
	writer.String(uInfo.avatar.c_str());
	writer.Key("male");
	writer.String(uInfo.male.c_str());
	writer.Key("country");
	writer.String(uInfo.country.c_str());
    }

    /* without '{', '}' at begin and end
      "uid": "uid",...
     */
    void ResultFormatter::formatUserResource(JsonWriter &writer, const UserResource &uRes) {
	writer.Key("uid");
	writer.String(uRes.uid.c_str());
	writer.Key("level");
	writer.Uint(uRes.level);
	writer.Key("exp");
	writer.Uint64(uRes.exp);
	writer.Key("fortune");
	writer.Uint64(uRes.fortune);
	writer.Key("vip_level");
	writer.Uint(uRes.vipLevel);
	// writer.Key("vip_point");
	// writer.String(uRes.vipPoint);
    }	
}
