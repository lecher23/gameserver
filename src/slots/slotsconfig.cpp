#include "slotsconfig.h"

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
        CLOG(ERROR) << "Get bet2exp config from db failed.";
        return false;
    }

    if(!(themeConfig.initConfig())) {
        CLOG(ERROR) << "Get Theme config from db failed.";
        return false;
    }
    return true;
}

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
    FIND_VAL_IN_MAP(bet2Exp, itr0, resource, 0);
    float ext = 0.0;
    FIND_VIP_CONFIG(context.user->uRes.vipLevel, ext, exp_ext, 0.0);
    return itr0->second * (1.0 + ext);
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
