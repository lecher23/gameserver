#ifndef SOCIALITYSERVICE_H
#define SOCIALITYSERVICE_H

#include "slotsdb.h"
#include "iservice.h"

namespace slots{
    class SocialityService: IService{
    public:
        SocialityService();
        virtual ~SocialityService();

	virtual bool doJob(CPacket &packet, CResponse &resp);
    private:
	bool getFriendsList(CPacket &packet, ResultFormatter &rf);
	bool getSearchResult(CPacket &packet, ResultFormatter &rf);	
    };
}
#endif
