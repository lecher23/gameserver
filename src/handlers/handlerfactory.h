#ifndef HANDLERFACTORY_H
#define HANDLERFACTORY_H

#include "ihandler.h"
#include "packethandler.h"
namespace cgserver{
    class HandlerFactory{
    public:
        HandlerFactory();
        ~HandlerFactory();

	static IHandler* getHandler() {
	    return (IHandler *)new cglogic::PacketHandler;
	}
    };
}
#endif
