#include "themeconfig.h"
#include "util/config.h"

BEGIN_NAMESPACE(slots)

ThemeConfig::ThemeConfig(){
}

ThemeConfig::~ThemeConfig(){
}

bool ThemeConfig::initConfig() {
    return getTemplePrincessConfig();
}

#define PARSE_JSON_CONFIG_HELPER(path, target)                          \
    if (!target.initFromJsonFile(path)){                                \
        CLOG(ERROR) << "parse json config:" << path << " failed.";      \
        break;                                                          \
    }

#define PARSE_WARPPER(class_name, instance, path, func)                 \
    class_name instance;                                                \
    PARSE_JSON_CONFIG_HELPER(path, instance);                           \
    if(!func(instance)) {                                               \
        CLOG(ERROR) << "convert config from path " << path << " failed."; \
        break;                                                          \
    }


bool ThemeConfig::getTemplePrincessConfig() {
    auto &cfg = cgserver::Config::getInstance();
    auto &db = SlotsDB::getInstance();
    bool ret = false;
    do {
        // common config: some are in db.
        ThemeCommonConfig tcc;
        if (!db.getThemeCommonConfig(tcc)) {
            CLOG(ERROR) << "get theme config from db failed.";
            break;
        }
        if (!getCommonConfig(tcc)) {
            CLOG(ERROR) << "parse theme config failed.";
            break;
        }
        // grids setting
        GridsConfig gc;
        PARSE_JSON_CONFIG_HELPER(cfg.getTpGridsCfgPath(), gc);
        if(!getGridConfig(gc, false)) {
            CLOG(ERROR) << "parse grid config from db failed.";
            break;
        }
        // free game grid setting
        GridsConfig fgc;
        PARSE_JSON_CONFIG_HELPER(cfg.getTpFreegameGridsCfgPath(), fgc);
        if(!getGridConfig(fgc, true)) {
            CLOG(ERROR) << "convert free game grid config failed.";
            break;
        }
        // lines config
        PARSE_WARPPER(SlotLinesConfig, lineCfg, cfg.getTpLinesCfgPath(), getLinesConfig);
        // element config
        PARSE_WARPPER(SlotEleConfig, eleCfg, cfg.getTpEleCfgPath(), getElements);
        // free game config
        PARSE_WARPPER(FreeGameConfig, freeGameCfg, cfg.getTpFreegameCfgPath(), getFreeGameConfig);
        // prize pool config
        PARSE_WARPPER(PrizePoolConfig, prizePoolCfg, cfg.getTpPrizepoolCfgPath(), getPrizePoolConfig);
        // butterfly config
        auto &butterfly = tsConfig.getButterfly();
        PARSE_JSON_CONFIG_HELPER(cfg.getButterflyConfigPath(), butterfly);
        ret = true;
    }while(false);
    return ret;
}

#undef PARSE_WARPPER
#undef PARSE_JSON_CONFIG_HELPER

bool ThemeConfig::getGridConfig(GridsConfig &gc, bool isFree) {
    if (!isFree) {
        tsConfig.setRowNumber(gc.getRowSize());
        tsConfig.setColumnNumber(gc.getColSize());
        tsConfig.setJackpot1Count(gc.getColSize());
        tsConfig.setJackpot2Count(gc.getColSize());
    }

    std::map<int32_t, TSGrid> grids;
    int32_t index;
    int len = gc.end();
    for (size_t i = 0; i < len; ++i) {
        auto &item = gc.getGrid(i); // row, colum, eleID, weight,
        auto &grid = tsConfig.getGrid(item.row, item.col, isFree);
        grid.addElement(item.eleID, item.weight);
    }
    // check
    if (!tsConfig.checkGrids()) {
        return false;
    }
    return true;
}

bool ThemeConfig::getLinesConfig(SlotLinesConfig &lc) {
    for (int i = lc.begin(); i < lc.end(); ++i) {
        tsConfig.addLine(i + 1, lc.getLine(i));
    }
    return true;
}

bool ThemeConfig::getElements(SlotEleConfig &cfg) {
    for (int i = cfg.begin(); i < cfg.end(); ++i) {
        const auto &ele = cfg.getElement(i);
        tsConfig.addElement(
            ele.id, ele.type, ele.repeatable);
        for (const auto &item: ele.reward) {
            tsConfig.setElementRatio(ele.id, item.first, item.second);
        }
    }
    tsConfig.getFreeGameConfig().setElementId(cfg.freeGameEleID);
    tsConfig.getTinyGameConfig().setElementId(cfg.tinyGameEleID);
    tsConfig.setJackpot1EleID(cfg.roomWinEleID);
    tsConfig.setJackpot2EleID(cfg.hallWinEleID);
    return true;
}

bool ThemeConfig::getFreeGameConfig(FreeGameConfig &cfg) {
    for (int i = cfg.begin(); i < cfg.end(); ++i) {
        const auto &item = cfg.getItem(i);
        tsConfig.getFreeGameConfig().addConfig(item.first, item.second);
    }
    return true;
}

bool ThemeConfig::getPrizePoolConfig(PrizePoolConfig &cfg) {
    const auto &hallCfg = cfg.getHallConfig();
    tsConfig.setTax4Hall(hallCfg.tax);
    tsConfig.setMinHallPrizePool(hallCfg.base);
    tsConfig.setJackpot2Limit(hallCfg.minBet);
    const auto &roomCfg = cfg.getRoomConfig();
    tsConfig.setTax4Room(roomCfg.tax);
    tsConfig.setMinRoomPrizePool(roomCfg.base);
    tsConfig.setJackpot1Limit(roomCfg.minBet);
    return true;
}

bool ThemeConfig::getCommonConfig(ThemeCommonConfig &cfg) {
    // code, value, extra
    int32_t code;
    for (auto &sCfg: cfg) {
        code = sCfg.code;
        if (code < ROOM_CFG_END) {
            switch(code) {
            case ROOM_RESERVE_TIME_CODE:{
                tsConfig.setRoomReserveTime(sCfg.value);
                break;
            }
            case ROOM_FORCE_WIN_CODE:{
                tsConfig.setForceWinRoomPrize(sCfg.value);
                break;
            }
            }
        }
        else if (code < HALL_CFG_END) {
            switch(code) {
            case HALL_FORCE_WIN_CODE: {
                tsConfig.setForceWinHallPrize(sCfg.value);
                break;
            }
            }
        } else if (code < WIN_CFG_END) {
            if (code == WIN_MEGA_CODE) {
                tsConfig.setMegawin(sCfg.value);
            }else if (code == WIN_BIG_CODE) {
                tsConfig.setBigwin(sCfg.value);
            }else if (code == WIN_SUPER_CODE) {
                tsConfig.setSuperwin(sCfg.value);
            }
        } else {
            CLOG(WARNING) << "invalid config code:" << code;
        }
    }
    return true;
}

END_NAMESPACE
