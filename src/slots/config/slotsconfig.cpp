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

    GET_CONFIG_FROM_JSON_FILE("slots", "achievement_cfg", loginCfg);
    GET_CONFIG_FROM_JSON_FILE("slots", "login_cfg", loginCfg);
    GET_CONFIG_FROM_JSON_FILE("slots", "level_cfg", levelConfig);

    if (!db.getVipSetting(vipSetting)) {
        CLOG(ERROR) << "Get vip config from db failed.";
        return false;
    }

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

#define FIND_VAL_IN_MAP(mp, dest, key, rt)      \
    auto dest = mp.find(key);                   \
    if (dest == mp.end()) {                     \
        return rt;                              \
    }

#define FIND_VIP_CONFIG(key, val, dest, df)     \
    auto itr = vipSetting.find(key);            \
    if (itr == vipSetting.end()) {              \
        val = df;                               \
    }else {                                     \
        val = itr->second.dest;                 \
    }

int64_t SlotsConfig::expGain(GameContext &context, int64_t resource) {
    auto exp = bet2Exp.getExp(resource);
    float ext = 0.0;
    FIND_VIP_CONFIG(context.user->uRes.vipLevel, ext, exp_ext, 0.0);
    return exp * (1.0 + ext);
}

int64_t SlotsConfig::earned(GameContext &context, int64_t src) {
    float ext = 0.0;
    FIND_VIP_CONFIG(context.user->uRes.vipLevel, ext, bounus_ext, 0.0);
    return src * (1.0 + ext);
}

int64_t SlotsConfig::vipLoginReward(int32_t level, int64_t src) {
    float ext = 0.0;
    FIND_VIP_CONFIG(level, ext, login_ext, 0.0);
    return src * ext;
}

END_NAMESPACE
