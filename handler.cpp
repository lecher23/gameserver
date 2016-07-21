#include "handler.h"
#include "packethandler.h"
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>

namespace cgserver{
    Handler::Handler() {
	;
    }
    void Handler::handle(DataBuffer &buf, int fd) {
	_parser.reset();
	HTTPPacket packet;
	_parser.processData(&buf, &packet);

	cglogic::PacketHandler phandler;
	phandler.init();
	HttpResponsePacket resp;
	phandler.process(packet, resp);
	// do not use private attribute. notice multi thread problem.
	cgserver::DataBuffer output; 
	if (resp.encode(&output)) {
	    ::send(fd, output.getData(), output.getDataLen(), 0);
	}
	::close(fd);
	std::cout << "finish send packet" << std::endl;
    }
}
