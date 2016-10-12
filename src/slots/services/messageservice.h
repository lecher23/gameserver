#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "iservice.h"

BEGIN_NAMESPACE(slots)

class MessageService: public IService{
public:
    MessageService();
    virtual ~MessageService();
    virtual bool doJob(CPacket &packet, CResponse &resp);

private:
    bool enterRoom(CPacket &packet, ResultFormatter &rf);
    bool queryRoomPrize(CPacket &packet, ResultFormatter &rf);
    bool queryAllRoomInHall(CPacket &packet, ResultFormatter &rf);
    bool getLoginReward(CPacket &packet, int64_t &newFortune);
    bool getCargoStatus(CPacket &packet);
};
END_NAMESPACE
#endif
