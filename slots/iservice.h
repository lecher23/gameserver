#ifndef ISERVICE_H
#define ISERVICE_H

#include "../handlers/ihandler.h"
#include "../util/luatoolfactory.h"
#include "../util/common_define.h"
#include "../util/stringutil.h"

#include "resultformatter.h"

#define GET_PARAM(key, dest, required)					\
    if (!packet.getParamValue(key, dest)) {				\
	if (required) {							\
	    CLOG(ERROR) << "Get [" << key << "] from url failed.";	\
	    return false;						\
	}else {								\
	    dest.clear();						\
	}								\
    }

#define GET_PARAM_WITH_BREAK(key, dest, required)					\
    if (!packet.getParamValue(key, dest)) {				\
	if (required) {							\
	    CLOG(ERROR) << "Get [" << key << "] from url failed.";	\
	    break;						\
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
