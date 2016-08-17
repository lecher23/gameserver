#ifndef MAILSERVICE_H
#define MAILSERVICE_H

#include <slots/sql/slotsdb.h>
#include "iservice.h"

namespace slots{
    class MailService:IService{
    public:
        MailService();
        virtual ~MailService();
	
	virtual bool doJob(CPacket &packet, CResponse &resp);
    };
}
#endif
