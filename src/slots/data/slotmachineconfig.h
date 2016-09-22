#ifndef SLOTMACHINECONFIG_H
#define SLOTMACHINECONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct EleChance {
  int32_t eleID;
  int32_t weight;
 EleChance():weight(0){}
};

struct SlotGrid {
  std::vector<EleChance> elements;
  int32_t totalWeight;
 SlotGrid():totalWeight(0){}
};

/* struct LineConfig { */
/*   std::vector<int32_t> line;// line[0] = ele_position */
/* }; */
typedef std::vector<int32_t> SlotLine;

struct SlotElementRatio {
  int32_t type;
  /*key: line number, value: reward. */
  std::map<int32_t, int64_t> ratio;
};

struct SlotMachineConfig {
  int32_t maxRow;
  int32_t maxColumn;
  std::map<int32_t, SlotGrid> grids;
  std::vector<SlotLine> lines; // line1, line2, line3
  std::map<int32_t, SlotElementRatio> elements; // key: elment id
  bool bEleRepeatInCol;
 SlotMachineConfig():maxRow(0), maxColumn(0),bEleRepeatInCol(false) {}
  int32_t toGridIdx(int32_t row, int32_t col) {return row * maxColumn + col;}
};

END_NAMESPACE
#endif
