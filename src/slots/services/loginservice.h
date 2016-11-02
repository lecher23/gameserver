#ifndef LOGINSERVICE_H
#define LOGINSERVICE_H

#include "iservice.h"
#include <slots/processors/allprocessors.h>

namespace slots{
    class LoginService:IService{
    public:
        LoginService();
        virtual ~LoginService();

	virtual bool doJob(CPacket &packet, CResponse &resp);
    private:
	bool getUserInfo(CPacket &packet, GameContext &su) const;

        LoginProcessor _lProcessor;
        AchievementProcessor _aProcessor;
    };
}
#endif
