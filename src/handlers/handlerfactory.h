#ifndef HANDLERFACTORY_H
#define HANDLERFACTORY_H

#include "ihandler.h"
#include "../slots/slotshandler.h"

namespace cgserver{
    class HandlerFactory{
    public:
        HandlerFactory();
        ~HandlerFactory();

	static IHandler* getHandler() {
	    return (IHandler *)new slots::SlotsHandler;
	}
    };
}
#endif
