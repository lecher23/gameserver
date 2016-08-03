#include "mailservice.h"
#include "resultformatter.h"

namespace slots{
    MailService::MailService(){
    }
    
    MailService::~MailService(){
    }

    bool MailService::doJob(CPacket &packet, CResponse &resp) {
	std::string type;
	std::string uid;
	std::string page;
	std::string pageSize;
	std::string body;
	std::string errMsg;
	GET_PARAM("type", type, true);
	GET_PARAM("uid", uid, true);
	CLOG(INFO) << "Get mails for user [" << uid << "]";
	GET_PARAM("page", page, false);
	GET_PARAM("pageSize", pageSize, false);
	page = page.empty() ? "0":page;
	pageSize = pageSize.empty() ? "10":pageSize;
	SlotsDB &db = SlotsDB::getInstance();
	ResultFormatter rf;
	SBuf bf;
	bool ret = false;
	do {
	    if (type == "0") {
		UserMails uMails;
		if (!db.getUserMails(uid, page, pageSize, uMails)){
		    CLOG(WARNING) << "process /slots/mail?type=0 failed on [mysql]";
		    rf.formatFailed(bf);
		    break;
		}
		rf.formatResult(uMails, bf);
	    }
	}while(false);
	resp.setBody(bf.GetString());
	return ret;
    }
}
