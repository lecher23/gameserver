#include "templeprincess.h"
#include <time.h>
#include <stdlib.h>

BEGIN_NAMESPACE(slots)

TemplePrincess::TemplePrincess(TSConfig &cfg):_cfg(cfg) {
    srand(time(nullptr));
}

TemplePrincess::~TemplePrincess(){
}

// return bool
bool TemplePrincess::play(TSResult &data) const {
    size_t i = 0;
    auto tBet = data.bet * data.lineNumber;
    if (tBet >= _cfg.getJackpot1Limit()) {
        data.bJackpot1 = true;
    }
    if (tBet >= _cfg.getJackpot2Limit()) {
        data.bJackpot2 = true;
    }
    auto row = _cfg.getRowNumber();
    for (int32_t i = 0; i < row; ++i) {
        if (!chooseElementInColumn(i, data)) {
            return false;
        }
    }
    countLines(data);
    processLines(data);
    processSpecial(data);
    return true;
}

bool TemplePrincess::chooseElementInColumn(int32_t row, TSResult &data) const
{
    auto maxColumn = _cfg.getColumnNumber();
    std::set<int32_t> forbidPool;
    if(!data.bJackpot1) {
        forbidPool.insert(_cfg.getJackpot1EleID());
    }
    if(!data.bJackpot2) {
        forbidPool.insert(_cfg.getJackpot2EleID());
    }
    int32_t index;
    int32_t rd;
    for (int32_t i = 0; i < maxColumn; ++i) {
        index = _cfg.getGridIndex(row, i);
        auto &grid = _cfg.getGrid(index, data.bFreeGame);

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

void TemplePrincess::countLines(TSResult &data) const {
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
        for (pointPos = 1;
             pointPos < pointCount && VALID_ELEMENT_TYPE(preType); ++pointPos)
        {
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
            lines.push_back(LineDetail(line.getID(), preID, pointPos));
        }
    }
}
#undef VALID_ELEMENT_TYPE
#undef GET_ELEID_TYPE

void TemplePrincess::processLines(TSResult &data) const {
    for (auto &line: data.lines) {
        data.totalRatio += _cfg.getElementRatio(line.eleID, line.count);
        if (!data.bJackpot1)
        {
            data.bJackpot1 = _cfg.isJackpot1(line.lineID, line.count);
        }
        if (!data.bJackpot2) {
            data.bJackpot2 = _cfg.isJackpot2(line.lineID, line.count);
        }
    }
    data.earned = data.totalRatio * data.bet;
}

void TemplePrincess::processSpecial(TSResult &data) const {
    auto &fgc = _cfg.getFreeGameConfig();
    auto fid = fgc.getElementID();
    auto tid = _cfg.getTinyGameID();
    int32_t fCount = 0;
    int32_t tCount = 0;
    for(auto &item: data.gridsData) {
        if (item.second == fid) {++fCount;}
        else if (item.second == tid) {++tCount;}
    }
    data.freeGameTimes = fgc.getFreeGameConfig(fCount);
    data.tinyGameEleID = tid;
    data.tinyGameEleCount = tCount;

    int32_t mid = data.earned/(data.bet + data.lineNumber);
    data.bMegawin = _cfg.isMegawin(mid);
    data.bBigwin = _cfg.isBigwin(mid);
    data.bSuperwin = _cfg.isSuperwin(mid);
}

END_NAMESPACE
