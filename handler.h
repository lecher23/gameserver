#ifndef HANDLER_H
#define HANDLER_H

#include "databuffer.h"
#include "httpresponsepacket.h"
#include "httppacketparser.h"

namespace cgserver{
    class Handler{
    public:
	Handler();
	void handle(DataBuffer &buffer, int fd);
    private:
	HTTPPacketParser _parser;
    };

}

#endif
