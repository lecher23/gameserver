#include "themeconfig.h"

BEGIN_NAMESPACE(slots)

ThemeConfig::ThemeConfig(){
}

ThemeConfig::~ThemeConfig(){
}

bool ThemeConfig::initConfig() {
    return getTemplePrincessConfig();
}

bool ThemeConfig::getTemplePrincessConfig() {
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
        ThemeCommonConfig tcc;
        if (!db.getThemeCommonConfig(tcc)) {
            break;
        }
        if (!getCommonConfig(tcc)) {
            break;
        }
        ret = true;
    }while(false);
    return ret;
}

bool ThemeConfig::getGridConfig(GridConfigs &gc) {
    tsConfig.setRowNumber(gc.rowNum);
    tsConfig.setColumnNumber(gc.columnNum);

    std::map<int32_t, TSGrid> grids;
    int32_t index;
    int len = gc.grids.size();
    for (size_t i = 0; i < len; ++i) {
        auto &item = gc.grids[i]; // row, colum, eleID, weight,
        auto &grid = tsConfig.getGrid(item.row, item.column, false);
        grid.addElement(item.eleID, item.weight);
    }
    // check
    if (!tsConfig.checkGrids()) {
        return false;
    }
    return true;
}

bool ThemeConfig::getLinesConfig(LinesConfig &lc) {
    for (auto &line: lc) {
        tsConfig.addLine(line.lineID, line.line);
    }
    return true;
}

bool ThemeConfig::getElements(SlotElements &elements, ElementsConfig &cfg) {
    for (auto &se: cfg) {
        tsConfig.addElement(
            se.eleID, elements[se.eleID].type, elements[se.eleID].bRepeat);
        tsConfig.setElementRatio(se.eleID, se.lineNum, se.value);
    }
    return true;
}

bool ThemeConfig::getCommonConfig(ThemeCommonConfig &cfg) {
    // code, value, extra
    int32_t code;
    for (auto &sCfg: cfg) {
        code = sCfg.code;
        if (code < ROOM_CFG_END) {
            switch(code) {
            case ROOM_TAX_CODE: {
                tsConfig.setTax4Room(sCfg.value);
                break;
            }
            case ROOM_MIN_PRIZE_CODE: {
                tsConfig.setMinRoomPrizePool(sCfg.value);
                break;
            }
            case ROOM_ROLL_ELE_CODE: {
                tsConfig.setJackpot1EleID(sCfg.value);
                break;
            }
            case ROOM_ROLL_ELE_COUNT_CODE: {
                tsConfig.setJackpot1Count(sCfg.value);
                break;
            }
            case ROOM_ENABLE_REPEAT_ELE_CODE: {
                tsConfig.setRepeatElementEnabled(sCfg.value != 0);
                break;
            }
            case ROOM_RESERVE_TIME_CODE:{
                tsConfig.setRoomReserveTime(sCfg.value);
                break;
            }
            case ROOM_FORCE_WIN_CODE:{
                tsConfig.setForceWinRoomPrize(sCfg.value);
                break;
            }
            case ROOM_MIN_BET_TO_ROLL_CODE:{
                tsConfig.setJackpot1Limit(sCfg.value);
                break;
            }
            }
        }
        else if (code < HALL_CFG_END) {
            switch(code) {
            case HALL_TAX_CODE: {
                tsConfig.setTax4Hall(sCfg.value);
                break;
            }
            case HALL_MIN_PRIZE_CODE: {
                tsConfig.setMinHallPrizePool(sCfg.value);
                break;
            }
            case HALL_ROLL_ELE_CODE: {
                tsConfig.setJackpot2EleID(sCfg.value);
                break;
            }
            case HALL_ROLL_ELE_COUNT_CODE: {
                tsConfig.setJackpot2Count(sCfg.value);
                break;
            }
            case HALL_FORCE_WIN_CODE: {
                tsConfig.setForceWinHallPrize(sCfg.value);
                break;
            }
            }
        } else if (code < FREE_GAME_CFG_END) {
            if (code == FREE_GAME_ELE_CODE) {
                tsConfig.getFreeGameConfig().setElementId(sCfg.value);
                continue;
            }
            int32_t count = code - FREE_GAME_ELE_CODE;
            tsConfig.getFreeGameConfig().addConfig(count, sCfg.value);
        } else if (code < TINY_GAME_CFG_END) {
            if (code == TINY_GAME_ELE_CODE) {
                tsConfig.getTinyGameConfig().setElementId(sCfg.value);
                continue;
            }
            int32_t count = code - TINY_GAME_ELE_CODE;
            tsConfig.getTinyGameConfig().addConfig(count, sCfg.value);
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
