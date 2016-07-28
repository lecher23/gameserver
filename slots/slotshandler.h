#ifndef SLOTSHANDLER_H
#define SLOTSHANDLER_H

#include "../handlers/ihandler.h"

namespace slots{
    class SlotsHandler:cgserver::IHandler{
    public:
        SlotsHandler();
        virtual ~SlotsHandler();

	virtual bool process(cgserver::HTTPPacket &packet,
			     cgserver::HttpResponsePacket &resp);
	virtual void init();
    };
}
#endif
