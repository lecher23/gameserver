#ifndef SLOTSHANDLER_H
#define SLOTSHANDLER_H

#include "../handlers/ihandler.h"

namespace slots{
    class SlotsHandler:cgserver::IHandler{
    public:
	//typedef std::shared_ptr<cgserver::LruCache<std::string, CacheData> > LruCachePtr;
	
        SlotsHandler();
        virtual ~SlotsHandler();

	virtual bool process(cgserver::HTTPPacket &packet,
			     cgserver::HttpResponsePacket &resp);
	virtual bool init();

	virtual void release();
    };
}
#endif
