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
        tsConfig.addElement(se.eleID, elements[se.eleID]);
        tsConfig.setElementRatio(se.eleID, se.lineNum, se.value);
    }
    return true;
}

END_NAMESPACE
