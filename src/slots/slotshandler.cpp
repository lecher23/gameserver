#include <util/timeutil.h>
#include "slotshandler.h"
#include <slots/services/serviceprovider.h>
#include <slots/data/slotsdatacenter.h>
#include <slots/slotsconfig.h>

BEGIN_NAMESPACE(slots)
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

bool SlotsHandler::init() {
    if (!cgserver::MysqlConnPool::getInstance().readyToUse()) {
	CLOG(ERROR) << "Mysql conn pool not ready.";	
	return false;
    }
    if(!SlotsDataCenter::instance().init()){
	CLOG(ERROR) << "Init slots cache failed.";
	return false;
    }
    if(!SlotsConfig::getInstance().init()){
	CLOG(ERROR) << "Init game config failed.";
	return false;
    }
    CLOG(INFO) << "Init slots cache success.";
    return true;
}

void SlotsHandler::release() {
    SlotsDataCenter::instance().release();
}
END_NAMESPACE
