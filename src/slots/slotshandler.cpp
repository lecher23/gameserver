#include "slotshandler.h"
#include <util/config.h>
#include <util/timeutil.h>
#include <slots/services/serviceprovider.h>
#include <slots/data/slotsdatacenter.h>
#include <slots/config/slotsconfig.h>
#include <cache/redisclientfactory.h>

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
    auto &redisClient = cgserver::RedisClientFactory::getClient();
    auto &cfg = cgserver::Config::getInstance();
    // timeout:2, pool_size:3
    if (cfg.enableRedis() &&
        !redisClient.Initialize(cfg.getRedisHost(), cfg.getRedisPort(), 2, 3))
    {
        CLOG(ERROR) << "Init Redis client failed.";
        return false;
    }
    CLOG(INFO) << "Init slots cache success.";
    return true;
}

void SlotsHandler::release() {
    SlotsDataCenter::instance().release();
}
END_NAMESPACE
