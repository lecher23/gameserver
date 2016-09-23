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
    auto row = _cfg.getRowNumber();
    for (int32_t i = 0; i < row; ++i) {
        if (!chooseElementInColumn(i, data)) {
            return false;
        }
    }
    countLines(data);
    return true;
}

bool TemplePrincess::chooseElementInColumn(int32_t column, GameResultData &data) const
{
    auto maxRow = _cfg.getRowNumber();
    std::set<int32_t> forbidPool;
    int32_t index;
    int32_t rd;
    for (int32_t i = 0; i < maxRow; ++i) {
        index = _cfg.getGridIndex(i, column);
        auto &grid = _cfg.getGrid(index);

        srand((int)time(0) + rd * 193);
        rd = rand() % grid.getTotalWeight();

        auto eleID = locateElement(rd, forbidPool, grid);
        if (eleID < 0) {
            return false;
        }
        if (!_cfg.repeatElementEnabled()) {
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
    int32_t len = grid.getElementsCount();
    int32_t round = 0;
    while(true) {
        auto weight = grid.getElementWeight(idx);
        auto eleID = grid.getElementID(idx);
        curSum += (forbid.count(eleID) > 0 ? 0: weight);
        if(rd < curSum) {
            return eleID;
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
    ele_id = data.gridsData[line.getPoint(pointPos)];       \
    ele_type = _cfg.getElementType(ele_id);

#define VALID_ELEMENT_TYPE(ele_type) (ele_type == WILD_ELEMENT || ele_type == NORMAL_ELEMENT)

void TemplePrincess::countLines(GameResultData &data) const {
    auto &lines = data.lines;
    auto col = _cfg.getColumnNumber();
    int32_t preID, preType, curID, curType;
    int32_t linesCount = _cfg.getLinesCount();
    for (size_t i = 0; i < linesCount; ++i) {
        auto &line = _cfg.getLine(i);
        auto pointCount = line.getPointCount();
        if (pointCount != col || col == 0) {
            CLOG(INFO) << "Invalid column number";
            continue;
        }
        size_t pointPos = 0;
        GET_ELEID_TYPE(preID, preType);
        for (pointPos = 1; pointPos < pointCount && VALID_ELEMENT_TYPE(preType); ++pointPos) {
            GET_ELEID_TYPE(curID, curType);
            if (!VALID_ELEMENT_TYPE(curType)) {
                break;
            }
            if (preType == WILD_ELEMENT) {
                preType = curType;
                preID = curID;
            } else if (curType == NORMAL_ELEMENT && curID != preID) {
                break;
            }
        }
        if (pointPos > 1 && preType == NORMAL_ELEMENT) {
            lines[line.getID()].eleID = preID;
            lines[line.getID()].count = pointPos;
        }
    }
}
#undef VALID_ELEMENT_TYPE
#undef GET_ELEID_TYPE

END_NAMESPACE
