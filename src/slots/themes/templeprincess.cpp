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
    if (data.bet >= _cfg.getJackpot1Limit()) {
        data.bJackpot1 = true;
    }
    if (data.bet >= _cfg.getJackpot2Limit()) {
        data.bJackpot2 = true;
    }
    auto col = _cfg.getColumnNumber();
    auto row = _cfg.getRowNumber();
    for (int32_t i = 0; i < col; ++i) {
        if (!chooseElementInColumn(i, row, data)) {
            return false;
        }
    }
    countLines(data);
    processLines(data);
    processSpecial(data);
    playTinyGame(data);
    return true;
}

bool TemplePrincess::chooseElementInColumn(int32_t col, int32_t maxRow, TSResult &data) const
{
    std::set<int32_t> forbidPool;
    if(!data.bJackpot1) {
        forbidPool.insert(_cfg.getJackpot1EleID());
    }
    if(!data.bJackpot2) {
        forbidPool.insert(_cfg.getJackpot2EleID());
    }
    int32_t index;
    int32_t rd;
    for (int32_t i = 0; i < maxRow; ++i) {
        index = _cfg.getGridIndex(i, col);
        auto &grid = _cfg.getGrid(index, data.bFreeGame);

        rd = rand() % grid.getTotalWeight();

        auto eleID = locateElement(rd, forbidPool, grid);
        if (eleID < 0) {
            return false;
        }
        if (!_cfg.elementRepeatable(eleID)) {
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

#define GET_ELEID_TYPE(ele_id, ele_type)                \
    ele_id = data.gridsData[line.getPoint(pointPos)];   \
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
    data.bJackpot1 = data.bJackpot2 = false;
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
    data.earned.normal = data.totalRatio * data.bet / data.lineNumber;
}

void TemplePrincess::processSpecial(TSResult &data) const {
    auto &fgc = _cfg.getFreeGameConfig();
    auto fid = fgc.getElementID();
    auto &tGameCfg = _cfg.getTinyGameConfig();
    auto tEleID = tGameCfg.getElementID();
    int32_t fCount = 0;
    int32_t tCount = 0;
    for(auto &item: data.gridsData) {
        if (item.second == fid) {++fCount;}
        else if (item.second == tEleID) {++tCount;}
    }
    data.freeGameTimes += fgc.getConfig(fCount);
    data.tinyGame.eleID = tEleID;
    data.tinyGame.eleCount = tCount;
    auto tinyGameID = tGameCfg.getConfig(tCount);
    if (tinyGameID > 0) {
        data.tinyGame.enable = true;
        data.tinyGame.tinyGameID = tinyGameID;
    }

    int32_t mid = data.earned.normal/data.bet;
    data.bMegawin = _cfg.isMegawin(mid);
    data.bBigwin = _cfg.isBigwin(mid);
    data.bSuperwin = _cfg.isSuperwin(mid);
}

void TemplePrincess::playTinyGame(TSResult &ret) const {
    auto tg = ret.tinyGame;
    if (!tg.enable) {
        return;
    }
    auto selectCount = ret.tinyGame.tinyGameID;
    int32_t multiple = false;
    ret.tinyResult.clear();
    auto factor = _cfg.getButterfly().play(selectCount, multiple, ret.tinyResult);
    // if no multiple: vector end with 1
    ret.tinyResult.push_back(multiple);
    ret.earned.tinyGame = ret.bet * factor;
}

END_NAMESPACE
