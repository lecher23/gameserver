#include "slotmachine1.h"
#include <time.h>
#include <stdlib.h>


BEGIN_NAMESPACE(slots)

SlotMachine1::SlotMachine1(SlotMachineConfig &cfg):_cfg(cfg) {
}

SlotMachine1::~SlotMachine1(){
}

void SlotMachine1::play() {
    size_t i = 0;
    for (auto &item: _cfg.gridsConfig) {
        chooseElement(item);
        ++i;
    }
    countLines();
}

void SlotMachine1::chooseElement(GridConfig &cfg) {
    srand((int)time(0));
    int32_t val = rand() % cfg.totalWeight;
    for (auto &i: cfg.elements) {
        if (val >= i.begin && val < i.end) {
            _result.push_back(i.eleId);
            break;
        }
    }
}

void SlotMachine1::countLines() {
    for (auto &line: _cfg.linesConfig) {
        auto len = line.size();
        if (len == 0) {
            continue;
        }
        size_t i = 0;
        int32_t eleID = _result[line[i++]];
        // make sure that item.line.size() > item.line[i]
        for (; i < len && eleID == _result[line[i]]; ++i) {;}
        if (i == len) {
            newLine(eleID);
        }
    }
}

void SlotMachine1::newLine(int32_t eleID) {
    auto itr = _lineInfo.find(eleID);
    if (itr == _lineInfo.end()) {
        _lineInfo[eleID] = 1;
        return;
    }
    ++itr->second;
}

END_NAMESPACE
