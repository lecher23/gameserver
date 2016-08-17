#ifndef SOCIALITYSERVICE_H
#define SOCIALITYSERVICE_H

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
	bool addFriend(CPacket &packet, ResultFormatter &rf);
	bool sendGold(CPacket &packet, ResultFormatter &rf);
	bool takeGold(CPacket &packet, ResultFormatter &rf);
	bool getGiftList(CPacket &packet, ResultFormatter &rf);
	bool batchSendGold(CPacket &packet, ResultFormatter &rf);
	bool getReward(CPacket &packet, ResultFormatter &rf);
	bool removeFriend(CPacket &packet, ResultFormatter &rf);
	bool batchGetGold(CPacket &packet, ResultFormatter &rf);
	bool getAllInfo(CPacket &packet, ResultFormatter &rf);	
    };
}
#endif
