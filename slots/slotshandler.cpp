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
	    resp.setBody(common::ERR_HTTP_METHOD.c_str());
	    return false;
	}
	IServicePtr service = ServiceProvider::getService(packet.getPath());
	if (service.get() == NULL) {
	    resp.setBody(common::ERR_HTTP_URI.c_str());
	    return false;
	}
	return service->doJob(packet, resp);
    }

    void SlotsHandler::init() {
	;
    }
}
