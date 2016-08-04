#ifndef SLOTSHANDLER_H
#define SLOTSHANDLER_H

#include "../handlers/ihandler.h"
#include "slotslrucache.h"

namespace slots{
    class SlotsHandler:cgserver::IHandler{
    public:
	//typedef std::shared_ptr<cgserver::LruCache<std::string, CacheData> > LruCachePtr;
	typedef std::shared_ptr<SlotsLruCache> LruCachePtr;
	
        SlotsHandler();
        virtual ~SlotsHandler();

	virtual bool process(cgserver::HTTPPacket &packet,
			     cgserver::HttpResponsePacket &resp);
	virtual void init();

    };
}
#endif
