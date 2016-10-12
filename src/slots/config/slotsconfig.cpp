#include "slotsconfig.h"
#include <util/config.h>

BEGIN_NAMESPACE(slots)

#define GET_CONFIG_FROM_JSON_FILE(grp, section, dest)                   \
    {                                                                   \
        auto path =                                                     \
            cgserver::Config::getInstance().getConfigValue(grp, section); \
        if (!dest.initFromJsonFile(path)) {                             \
            CLOG(ERROR) << "init config for "<< section <<" failed.";   \
            return false;                                               \
        }                                                               \
    }

bool SlotsConfig::init(){
    auto &db = SlotsDB::getInstance();
    if (!db.getCargoInfo(cargoInfo)) {
        CLOG(ERROR) << "Get cargo info from db failed.";
        return false;
    }

    GET_CONFIG_FROM_JSON_FILE("slots", "achievement_cfg", cjConfig);
    GET_CONFIG_FROM_JSON_FILE("slots", "login_cfg", loginCfg);
    GET_CONFIG_FROM_JSON_FILE("slots", "level_cfg", levelConfig);
    GET_CONFIG_FROM_JSON_FILE("slots", "vip_cfg", vipSetting);

    if(!db.getBet2ExpSetting(bet2Exp)) {
        CLOG(ERROR) << "Get bet2exp config from db failed.";
        return false;
    }

    if(!(themeConfig.initConfig())) {
        CLOG(ERROR) << "Get Theme config from db failed.";
        return false;
    }
    return true;
}

#undef GET_CONFIG_FROM_JSON_FILE

END_NAMESPACE
