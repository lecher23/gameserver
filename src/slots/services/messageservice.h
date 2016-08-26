#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "iservice.h"

BEGIN_NAMESPACE(slots)

class MessageService{
public:
    MessageService();
    virtual ~MessageService();
    virtual bool doJob(CPacket &packet, CResponse &resp);

private:
};

END_NAMESPACE
#endif
