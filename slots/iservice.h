#ifndef ISERVICE_H
#define ISERVICE_H

#include "../handlers/ihandler.h"
#include "../util/luatoolfactory.h"
#include "../util/common_define.h"

#define GET_PARAM(key, dest, required)					\
    if (!packet.getParamValue(key, dest)) {				\
	if (required) {							\
	    CLOG(ERROR) << "Get [" << key << "] from url failed.";	\
	    errMsg = "Code:001";					\
	    return false;						\
	}else {								\
	    dest.clear();						\
	}								\
    }


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
