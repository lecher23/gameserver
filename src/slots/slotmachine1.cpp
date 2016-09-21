#include "slotmachine1.h"
#include <time.h>
#include <stdlib.h>


BEGIN_NAMESPACE(slots)

SlotMachine1::SlotMachine1(SlotMachineConfig &cfg):_cfg(cfg) {
}

SlotMachine1::~SlotMachine1(){
}

void SlotMachine1::play(GameResultData &data) const {
    size_t i = 0;
    for (auto &item: _cfg.grids) {
        chooseElement(item.first, item.second, data);
        ++i;
    }
    countLines(data);
    collectSpecial(data);
}

void SlotMachine1::chooseElement(
    int32_t gridIdx, SlotGrid &cfg, GameResultData &data) const
{
    srand((int)time(0));
    int32_t val = rand() % cfg.totalWeight;
    for (auto &i: cfg.elements) {
        if (val >= i.begin && val < i.end) {
            data.gridsData[gridIdx] = i.eleID;
            break;
        }
    }
}

void SlotMachine1::countLines(GameResultData &data) const {
    auto &lines = data.lines;
    for (auto &line: _cfg.lines) {
        auto len = line.size();
        if (len == 0) {
            continue;
        }
        size_t i = 0;
        int32_t eleID = data.gridsData[line[i++]];
        // make sure that item.line.size() > item.line[i]
        for (; i < len && eleID == data.gridsData[line[i]]; ++i) {;}
        if (i == len) {
            auto itr = lines.find(eleID);
            if (itr == lines.end()) {
                lines[eleID] = 1;
            } else {
                ++(itr->second);
            }
        }
    }
}

void SlotMachine1::collectSpecial(GameResultData &data) const {
    // TODO: big win, megawin
    return;
}

END_NAMESPACE
