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
    bool getLoginReward(CPacket &packet);
    bool getCargoStatus(CPacket &packet);
};
END_NAMESPACE
#endif
