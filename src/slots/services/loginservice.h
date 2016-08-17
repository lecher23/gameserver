#ifndef LOGINSERVICE_H
#define LOGINSERVICE_H

#include "iservice.h"

namespace slots{
    class LoginService:IService{
    public:
        LoginService();
        virtual ~LoginService();

	virtual bool doJob(CPacket &packet, CResponse &resp);
    private:
	void formatResult(bool success, SlotsUser &su, std::string &out) const;
	bool getUserInfo(CPacket &packet, SlotsUser &su) const;
    };
}
#endif
