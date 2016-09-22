#include "templeprincess.h"
#include <time.h>
#include <stdlib.h>

BEGIN_NAMESPACE(slots)

TemplePrincess::TemplePrincess(TSConfig &cfg):_cfg(cfg) {
}

TemplePrincess::~TemplePrincess(){
}

// return bool
bool TemplePrincess::play(GameResultData &data) const {
    size_t i = 0;
    for (int32_t i = 0; i < _cfg.maxColumn; ++i) {
        if (!chooseElementInColumn(i, data)) {
            return false;
        }
    }
    countLines(data);
    return true;
}

bool TemplePrincess::chooseElementInColumn(int32_t column, GameResultData &data) const
{
    auto maxRow = _cfg.maxRow;
    std::set<int32_t> forbidPool;
    int32_t index;
    int32_t rd;
    for (int32_t i = 0; i < maxRow; ++i) {
        index = _cfg.toGridIdx(i, column);
        auto &grid = _cfg.grids[index];

        srand((int)time(0) + rd * 193);
        rd = rand() % grid.totalWeight;

        auto eleID = locateElement(rd, forbidPool, grid);
        if (eleID < 0) {
            return false;
        }
        if (!_cfg.bEleRepeatInCol) {
            forbidPool.insert(eleID);
        }
        data.gridsData[index] = eleID;
    }
    return true;
}

int32_t TemplePrincess::locateElement(
    int32_t rd, std::set<int32_t> &forbid, TSGrid &grid) const
{
    static const int32_t forceStopRound = 3;
    int32_t curSum = 0;
    int32_t idx = 0;
    int32_t len = grid.elements.size();
    int32_t round = 0;
    while(true) {
        auto &item = grid.elements[idx];
        curSum += (forbid.count(item.eleID) > 0 ? 0: item.weight);
        if(rd < curSum) {
            return item.eleID;
        }
        if (++idx == len) {
            idx = 0;
            ++ round;
        }
        if (round > forceStopRound) {
            break;
        }
    }
    CLOG(ERROR) << "Can not locate element with random value:" << rd;
    return -1;
}

#define GET_ELEID_TYPE(ele_id, ele_type)        \
    ele_id = data.gridsData[line[i]];           \
    ele_type = es[ele_id].type;

void TemplePrincess::countLines(GameResultData &data) const {
    auto &lines = data.lines;
    auto col = _cfg.maxColumn;
    auto &es = _cfg.elements;
    int32_t preID, preType, curID, curType;
    for (auto &item: _cfg.lines) {
        auto &line = item.second;
        auto len = line.size();
        if (len != col || col == 0) {
            CLOG(INFO) << "Invalid column number";
            continue;
        }
        size_t i = 0;
        GET_ELEID_TYPE(preID, preType);
        for (i = 1; i < len; ++i) {
            GET_ELEID_TYPE(curID, curType);
            if (preType == WILD_ELEMENT_TYPE) {
                preType = curType;
                preID = curID;
            }else if (curType != WILD_ELEMENT_TYPE && curID != preID) {
                break;
            }
        }
        if (i > 1) {
            lines[item.first].eleID = preID;
            lines[item.first].count = i;
        }
    }
}
#undef GET_ELEID_TYPE

END_NAMESPACE
