#ifndef LOGINSERVICE_H
#define LOGINSERVICE_H

#include "slotsdb.h"
#include "iservice.h"

namespace slots{
    class LoginService:IService{
    public:
        LoginService();
        virtual ~LoginService();

	virtual bool doJob(CPacket &packet, CResponse &resp);
    private:
	void formatResult(bool success, UserInfo &ui, std::string &out);
	bool getUserInfo(CPacket &packet, UserInfo &uinfo);
    };
}
#endif
