#ifndef RESULTFORMATTER_H
#define RESULTFORMATTER_H

#include "slotsdb.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <sstream>

namespace slots{
    typedef rapidjson::StringBuffer SBuf;
    typedef rapidjson::Writer<SBuf> JsonWriter;
    class ResultFormatter{
    public:
        ResultFormatter();
        ~ResultFormatter();

	void formatResult(const UserMails &uMails, SBuf &buffer);
	void formatResult(const SlotsUser &su, SBuf &buffer);
	void formatSimpleResult(SBuf &buffer, bool ok, const std::string &err);
	
    private:
	void formatStatus(JsonWriter &writer, bool bOk);
	void formatMailsInfo(JsonWriter &writer, const UserMails &uMails);
	void formatMail(JsonWriter &writer, const UserMail &uMail);
	void formatUserInfo(JsonWriter &writer, const UserInfo &uInfo);
	void formatUserResource(JsonWriter &writer, const UserResource &uRes);
    };
}
#endif
