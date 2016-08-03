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

    void ResultFormatter::formatFailed(SBuf &buffer){
	JsonWriter writer(buffer);	
	writer.StartObject();
	formatStatus(writer, false);
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
}
