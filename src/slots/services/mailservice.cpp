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
	GET_PARAM(slotconstants::sType, type, true);
	GET_PARAM(slotconstants::sUserID, uid, true);

	SlotsDB &db = SlotsDB::getInstance();
	SBuf bf;
	ResultFormatter rf(bf);
	bool ret = false;
	do {
	    if (type == "1") {
		std::string page;
		std::string pageSize;
		GET_PARAM(slotconstants::sPage, page, false);
		GET_PARAM(slotconstants::sPageSize, pageSize, false);
		page = page.empty() ? "0":page;
		pageSize = pageSize.empty() ? "10":pageSize;
		UserMails uMails;
		if (!db.getUserMails(uid, page, pageSize, uMails)){
		    CLOG(WARNING) << "process /slots/mail?type=0 failed on [mysql]";
		    rf.formatSimpleResult(false, "Internal error.");
		    break;
		}
		rf.formatMailList(uMails);
	    }else if (type == "2") {
		std::string mailId;
		GET_PARAM(slotconstants::sMailID, mailId, true);
		// use async?
		if (!db.delMail(uid, mailId)) {
		    CLOG(WARNING) << "delete mail from mysql failed";
		    rf.formatSimpleResult(false, "Internal error.");
		    break;
		}
		rf.formatSimpleResult(true, "");
	    }
            ret = true;
	}while(false);
	resp.setBody(bf.GetString(), bf.GetSize());
	return ret;
    }
}
