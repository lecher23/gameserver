#ifndef I_HANDLER_H
#define I_HANDLER_H

#include "../http/httppacket.h"
#include "../http/httpresponsepacket.h"
#include "../util/common_define.h"

namespace cgserver{
    /*Each handler will used by serveral thread.
      So it is import to note that do not add variable to IHandler.*/
    class IHandler{
    public:
	IHandler(){}
	virtual ~IHandler(){}
	virtual bool process(HTTPPacket &packet, HttpResponsePacket &resp) = 0; // we should add const later
	virtual void init() {}
    };

    DF_SHARED_PTR(IHandler);
}
#endif
