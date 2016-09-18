#include "slotsconfig.h"
#include <slots/sql/slotsdb.h>

BEGIN_NAMESPACE(slots)

bool SlotsConfig::init(){
    auto &db = SlotsDB::getInstance();
    // get achievement config from db
    if (!db.getAchivementSetting(cjConfig)){
        CLOG(ERROR) << "Get achievement setting from db failed";
	return false;
    }
    if (!db.getCargoInfo(cargoInfo)) {
        CLOG(ERROR) << "Get cargo info from db failed.";
        return false;
    }

    if (!db.getLoginSetting(loginCfg)) {
        CLOG(ERROR) << "Get login config from db failed.";
        return false;
    }
    int32_t preChance = 0;
    for(auto &item: loginCfg.runnerBonus) {
        item.second += preChance;
        preChance = item.second;
    }
    if (!db.getVipSetting(vipSetting)) {
        CLOG(ERROR) << "Get vip config from db failed.";
        return false;
    }

    if(!db.getLevelSetting(levelConfig)) {
        CLOG(ERROR) << "Get level config from db failed.";
        return false;
    }

    if(!db.getBet2ExpSetting(bet2Exp)) {
        CLOG(ERROR) << "Get level config from db failed.";
        return false;
    }
    return true;
}

int64_t SlotsConfig::expGain(int64_t resource) {
    auto itr = bet2Exp.find(resource);
    if (itr == bet2Exp.end()) {
        return 0;
    }
    return itr->second;
}

END_NAMESPACE
