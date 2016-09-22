#ifndef TSCONFIG_H
#define TSCONFIG_H

#include <util/common_define.h>
#include <slots/model/lineconfig.h>

BEGIN_NAMESPACE(slots)

struct EleChance {
  int32_t eleID;
  int32_t weight;
 EleChance():weight(0){}
};

struct TSGrid {
  std::vector<EleChance> elements;
  int32_t totalWeight;
 TSGrid():totalWeight(0){}
};

/* struct SlotLine { */
/*   std::vector<int32_t> line;// line[0] = ele_position */
/* }; */
typedef std::vector<int32_t> TSLine;

struct TSElementRatio {
  int32_t type;
  /*key: line number, value: reward. */
  std::map<int32_t, int64_t> ratio;
};

struct TSConfig {
  int32_t maxRow;
  int32_t maxColumn;
  std::map<int32_t, TSGrid> grids;
  std::map<int32_t, TSLine> lines; // key: lineID, val: lines
  std::map<int32_t, TSElementRatio> elements; // key: elment id
  bool bEleRepeatInCol;
 TSConfig():maxRow(0), maxColumn(0),bEleRepeatInCol(false) {}
  int32_t toGridIdx(int32_t row, int32_t col) {return row * maxColumn + col;}
};

END_NAMESPACE
#endif
