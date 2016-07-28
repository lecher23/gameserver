#ifndef ISERVICE_H
#define ISERVICE_H

#include "../handlers/ihandler.h"
#include "../util/luatoolfactory.h"
#include "../util/common_define.h"

namespace slots{
    typedef cgserver::HTTPPacket CPacket;
    typedef cgserver::HttpResponsePacket CResponse;
    class IService{
    public:
        virtual ~IService() {}
	virtual bool doJob(CPacket &packet,CResponse &resp) =0;
    };
    DF_SHARED_PTR(IService);
}
#endif
