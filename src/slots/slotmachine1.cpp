#include "slotmachine1.h"
#include <time.h>
#include <stdlib.h>


BEGIN_NAMESPACE(slots)

SlotMachine1::SlotMachine1(SlotMachineConfig &cfg):_cfg(cfg) {
}

SlotMachine1::~SlotMachine1(){
}

// return bool
void SlotMachine1::play(GameResultData &data) const {
    size_t i = 0;
    for (int32_t i = 0; i < _cfg.maxColumn; ++i) {
        chooseElementInColumn(i, data);
    }
    // for (auto &item: _cfg.grids) {
    //     chooseElement(item.first, item.second, data);
    //     ++i;
    // }
    countLines(data);
    collectSpecial(data);
}

void SlotMachine1::chooseElementInColumn(int32_t column, GameResultData &data) const
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
        if (!_cfg.bEleRepeatInCol) {
            forbidPool.insert(eleID);
        }
        data.gridsData[index] = eleID;
    }
}

int32_t SlotMachine1::locateElement(
    int32_t rd, std::set<int32_t> &forbid, SlotGrid &grid) const
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

void SlotMachine1::countLines(GameResultData &data) const {
    auto &lines = data.lines;
    auto col = _cfg.maxColumn;
    for (auto &item: _cfg.lines) {
        auto &line = item.second;
        auto len = line.size();
        if (len != col) {
            CLOG(INFO) << "Invalid column number";
            continue;
        }
        size_t i = 0;
        int32_t eleID = data.gridsData[line[i++]];
        // make sure that item.line.size() > item.line[i]
        for (; i < len && eleID == data.gridsData[line[i]]; ++i) {;}
        if (i > 1) {
            lines[item.first].eleID = eleID;
            lines[item.first].count = i;
        }
    }
}

void SlotMachine1::collectSpecial(GameResultData &data) const {
    // TODO: big win, megawin
    return;
}

END_NAMESPACE
