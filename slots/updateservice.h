#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include "slotsdb.h"
#include "iservice.h"

namespace slots{
    class UpdateService{
    public:
        UpdateService();
        virtual ~UpdateService();

	virtual bool doJob(CPacket &packet, CResponse &resp);
    private:
	bool updateUserInfo(CPacket &packet, std::string &errMsg);
	void formatResult(bool status, const std::string &msg, std::string &out);	
    };
}
#endif
