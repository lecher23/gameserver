#include "slotsconfig.h"
#include <util/config.h>

BEGIN_NAMESPACE(slots)

#define GET_CONFIG_FROM_JSON_FILE(path, dest)                           \
    if (!dest.initFromJsonFile(path)) {                                 \
        CLOG(ERROR) << "Init config for "<< path <<" failed.";     \
        return false;                                                   \
    }                                                                   \
    CLOG(INFO) << "Initilize config for " << path << " scuccess";

bool SlotsConfig::init(){
    auto &db = SlotsDB::getInstance();
    if (!db.getCargoInfo(cargoInfo)) {
        CLOG(ERROR) << "Get cargo info from db failed.";
        return false;
    }

    if(!(themeConfig.initConfig())) {
        CLOG(ERROR) << "Get Theme config from db failed.";
        return false;
    }

    auto &cfg = cgserver::Config::getInstance();
    GET_CONFIG_FROM_JSON_FILE(cfg.getAchievementConfigPath(), cjConfig);
    GET_CONFIG_FROM_JSON_FILE(cfg.getLoginConfigPath(), loginCfg);
    GET_CONFIG_FROM_JSON_FILE(cfg.getLevelConfigPath(), levelConfig);
    GET_CONFIG_FROM_JSON_FILE(cfg.getVipConfigPath(), vipSetting);
    GET_CONFIG_FROM_JSON_FILE(cfg.getOnlineConfigPath(), onlineConfig);

    return true;
}

#undef GET_CONFIG_FROM_JSON_FILE

END_NAMESPACE
