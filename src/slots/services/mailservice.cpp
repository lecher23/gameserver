#include "mailservice.h"

namespace slots{
    MailService::MailService(){
    }
    
    MailService::~MailService(){
    }

    bool MailService::doJob(CPacket &packet, CResponse &resp) {
	std::string type;
	std::string uid;
	std::string body;
	std::string errMsg;
	GET_PARAM("type", type, true);
	GET_PARAM("uid", uid, true);
	
	SlotsDB &db = SlotsDB::getInstance();
	SBuf bf;
	ResultFormatter rf(bf);
	bool ret = false;
	do {
	    if (type == "0") {
		std::string page;
		std::string pageSize;
		GET_PARAM("page", page, false);
		GET_PARAM("pageSize", pageSize, false);
		page = page.empty() ? "0":page;
		pageSize = pageSize.empty() ? "10":pageSize;
		UserMails uMails;
		if (!db.getUserMails(uid, page, pageSize, uMails)){
		    CLOG(WARNING) << "process /slots/mail?type=0 failed on [mysql]";
		    rf.formatSimpleResult(false, "Internal error.");
		    break;
		}
		rf.formatResult(uMails);
	    }else if (type == "3") {
		std::string mailId;
		GET_PARAM("mailid", mailId, true);
		// use async?
		if (!db.delMail(uid, mailId)) {
		    CLOG(WARNING) << "delete mail from mysql failed";
		    rf.formatSimpleResult(false, "Internal error.");
		    break;
		}
		rf.formatSimpleResult(true, "");
	    }
	    
	}while(false);
	resp.setBody(bf.GetString());
	return ret;
    }
}
