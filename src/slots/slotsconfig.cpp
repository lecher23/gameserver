#include "slotsconfig.h"
#include <slots/sql/slotsdb.h>

BEGIN_NAMESPACE(slots)

bool SlotsConfig::init(){
    int i;
    for (i = 1; i < 61; ++i) {
	LevelConfig lc;
	lc.level = i;
	lc.expNeed = i*139;
	lc.maxBet = i*10;
	lc.reward = i*50;
	levelConfig.push_back(lc);
    }
    for (i = 1; i < 11; ++i) {
	VipLevelConfig vlc;
	vlc.level = i;
	vlc.expPer = i*139;
	vlc.goldPer = i*10;
	vlc.goldNeed = i*50;
	vipLevelConfig.push_back(vlc);
    }

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
    return true;
}

const SlotsLevelConfig &SlotsConfig::getLevel(int64_t exp) {
    for (auto itr = levelConfig.begin(); itr != levelConfig.end(); ++itr) {
	if (exp < itr->expNeed) return *itr;
    }
    return levelConfig.back();
}

int64_t SlotsConfig::expGain(int64_t money) {
    // TODO: USE ARGORITHM
    return money;
}

int64_t SlotsConfig::expNeed2LevelUp(int64_t exp) {
    for (auto itr = levelConfig.begin(); itr != levelConfig.end(); ++itr) {
	if (exp < itr->expNeed) return (itr->expNeed - exp);
    }
    return (levelConfig.back().expNeed - exp);
}

const SlotsVipConfig &SlotsConfig::getVipLevel(int64_t vipPoint) {
    for (auto itr = vipLevelConfig.begin(); itr != vipLevelConfig.end(); ++itr) {
	if (vipPoint < itr->goldNeed) return *itr;
    }
    return vipLevelConfig.back();
}

END_NAMESPACE
