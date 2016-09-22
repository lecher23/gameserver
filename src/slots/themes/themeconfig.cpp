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
        ret = true;
    }while(false);
    return ret;
}

bool ThemeConfig::getGridConfig(GridConfigs &gc) {
    tsConfig.maxRow = gc.rowNum;
    tsConfig.maxColumn = gc.columnNum;

    std::map<int32_t, TSGrid> grids;
    int32_t index;
    int len = gc.grids.size();
    for (size_t i = 0; i < len; ++i) {
        auto &item = gc.grids[i];
        EleChance eChance;
        eChance.eleID = item.eleID;
        eChance.weight = item.weight;
        index = tsConfig.toGridIdx(item.row, item.column);
        grids[index].elements.push_back(eChance);
        grids[index].totalWeight += item.weight;
    }
    // check
    for (int32_t i = 0; i < tsConfig.maxRow; ++i) {
        for (int32_t j = 0; j < tsConfig.maxColumn; ++j) {
            index = tsConfig.toGridIdx(i, j);
            if(grids.find(index) == grids.end()) {
                return false;
            }
        }
    }
    tsConfig.grids.swap(grids);
    return true;
}

bool ThemeConfig::getLinesConfig(LinesConfig &lc) {
    for (auto &line: lc) {
        tsConfig.lines[line.lineID].swap(line.line);
    }
    return true;
}

bool ThemeConfig::getElements(SlotElements &elements, ElementsConfig &cfg) {
    for (auto &se: cfg) {
        TSElementRatio ec;
        ec.type = elements[se.eleID];
        ec.ratio[se.lineNum] = se.value;
        tsConfig.elements[se.eleID] = ec;
    }
    return true;
}

END_NAMESPACE
