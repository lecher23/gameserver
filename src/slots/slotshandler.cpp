#include "../util/timeutil.h"
#include "slotshandler.h"
#include "serviceprovider.h"
#include "slotsdatacenter.h"

namespace slots{
    SlotsHandler::SlotsHandler(){
    }
    
    SlotsHandler::~SlotsHandler(){
    }

    bool SlotsHandler::process(CPacket &packet, CResponse &resp)
    {
	auto beginTime = cgserver::CTimeUtil::getCurrentTime();

	if (CPacket::HM_POST != packet.getMethod()) {
	    CLOG(WARNING) << "Bad method: " << packet.getMethod();
	    return false;
	}
	IServicePtr service = ServiceProvider::getService(packet.getPath());
	if (service.get() == NULL) {
	    CLOG(WARNING) << "Invalid path: " << packet.getPath();
	    return false;
	}
	bool ret = service->doJob(packet, resp);
	auto endTime = cgserver::CTimeUtil::getCurrentTime();
	CLOG(INFO) << "process [" << packet.getPath() << "] cost:" << endTime - beginTime << " us.";
	return ret;
    }

    void SlotsHandler::init() {
	if(!SlotsDataCenter::instance().init()){
	    CLOG(WARNING) << "Init slots cache failed.";
	    return;
	}
	CLOG(INFO) << "Init slots cache success.";
    }
}
