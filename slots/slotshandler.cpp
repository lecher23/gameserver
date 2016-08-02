#include "slotshandler.h"
#include "serviceprovider.h"

namespace slots{
    SlotsHandler::SlotsHandler(){
    }
    SlotsHandler::~SlotsHandler(){
    }

    bool SlotsHandler::process(CPacket &packet, CResponse &resp)
    {
	if (CPacket::HM_POST != packet.getMethod()) {
	    CLOG(WARNING) << "Bad method: " << packet.getMethod();
	    return false;
	}
	IServicePtr service = ServiceProvider::getService(packet.getPath());
	if (service.get() == NULL) {
	    CLOG(WARNING) << "Invalid path: " << packet.getPath();
	    return false;
	}
	return service->doJob(packet, resp);
    }

    void SlotsHandler::init() {
	;
    }
}
