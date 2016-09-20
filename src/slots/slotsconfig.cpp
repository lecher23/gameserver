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
        CLOG(ERROR) << "Get level config from db failed.";
        return false;
    }

    if(!getSlotMachineConfig()) {
        CLOG(ERROR) << "Get level config from db failed.";
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

bool SlotsConfig::getSlotMachineConfig() {
    auto &db = SlotsDB::getInstance();
    bool ret = false;
    do {
        GridConfigs gc;
        if (!db.getGridsConfig(gc)){
            break;
        }
        if(!getGridConfig(gc)) {
            break;
        }
        LinesConfig lc;
        if (!db.getLinesConfig(lc)){
            break;
        }
        if(!getLinesConfig(lc)) {
            break;
        }
        ElementsConfig ec;
        if (!db.getElementsConfig(ec)){
            break;
        }
        SlotElements se;
        if (!db.getSlotsElements(se)){
            break;
        }
        if(!getElements(se, ec)) {
            break;
        }
        ret = true;
    }while(false);
    return ret;
}

bool SlotsConfig::getGridConfig(GridConfigs &gc) {
    std::map<int32_t, SlotGrid> grids;
    int32_t prev = 0;
    auto len = gc.size();
    /*if use for(auto &:) will meet problem, I should find out the reason.*/
    for (int i = 0; i < len; ++i){
        auto &grid = gc[i];
        EleChance eleChance;
        eleChance.eleID = grid.eleID;
        eleChance.begin = prev;
        prev += grid.weight;
        eleChance.end = prev;
        grids[grid.gridIdx].totalWeight += grid.weight;
        grids[grid.gridIdx].elements.push_back(eleChance);
    }
    slotConfig.grids.swap(grids);
    return true;
}

bool SlotsConfig::getLinesConfig(LinesConfig &lc) {
    for (auto &line: lc) {
        slotConfig.lines.push_back(line.line);
    }
    return true;
}

bool SlotsConfig::getElements(SlotElements &elements, ElementsConfig &cfg) {
    for (auto &se: cfg) {
        SlotElementRatio ec;
        CLOG(INFO) << se.eleID << "," << se.value << "," << se.lineNum;
        ec.type = elements[se.eleID];
        ec.ratio[se.lineNum] = se.value;
        slotConfig.elements[se.eleID] = ec;
    }
    return true;
}

END_NAMESPACE
