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
	bool getUserInfo(CPacket &packet, SlotsUser &su, Achievements &cj) const;
    };
}
#endif
