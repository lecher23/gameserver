#ifndef TSCONFIG_H
#define TSCONFIG_H

#include "util/common_define.h"
#include "butterfly.h"

BEGIN_NAMESPACE(slots)

#define  NORMAL_ELEMENT 0
#define  WILD_ELEMENT 1

struct EleChance {
  int32_t eleID;
  int32_t weight;
 EleChance():weight(0){}
 EleChance(int32_t id, int32_t w):eleID(id), weight(w){}
};

struct TSGrid {
  int32_t getElementsCount() { return elements.size();}
  int32_t getElementWeight(int32_t index) { return elements[index].weight;}
  int32_t getElementID(int32_t index) {return elements[index].eleID;}
  int32_t getTotalWeight() { return totalWeight;}

  void addElement(int32_t id, int32_t weight) {
    elements.push_back(EleChance(id, weight));
    totalWeight += weight;
  }

 TSGrid():totalWeight(0){}
 private:
  std::vector<EleChance> elements;
  int32_t totalWeight;
};

/* struct SlotLine { */
/*   std::vector<int32_t> line;// line[0] = ele_position */
/* }; */
class TSLine{
public:
  TSLine(){}
  ~TSLine(){}
  void setID(int32_t id_) {id = id_;}
  int32_t getID() { return id;}

  void addPoints(const std::vector<int32_t> &points) {
    _points.assign(points.begin(), points.end());
  }
  void addPoint(int32_t gridID) {
    _points.push_back(gridID);
  }
  int32_t getPoint(int32_t position) {
    return _points[position];
  }
  size_t getPointCount() {
    return _points.size();
  }

  void reset() {
    id = 0;
    _points.clear();
  }
private:
  int32_t id;
  std::vector<int32_t> _points;
};

struct TSElementRatio {
  void setType(int32_t t) { _type = t;}
  int32_t getType() { return _type;}

  void setRepeatable(bool val) {_bRepeat = val;}
  bool repeatable() {return _bRepeat;}

  void setRatio(int32_t lineCount, int32_t ratio) {
    _ratio[lineCount] = ratio;
  }
  int32_t getRatio(int32_t lineCount) {
    auto itr = _ratio.find(lineCount);
    if (itr != _ratio.end()) {
      return itr->second;
    }
    return 0;
  }
 private:
  int32_t _type{0};
  bool _bRepeat{false};
  /*key: line number, value: reward. */
  std::map<int32_t, int32_t> _ratio;
};

struct TinyGameConfig{
  void setElementId(int32_t id) { elementID = id;}
  int32_t getElementID() { return elementID;}
  int32_t getConfig(int32_t count) {return count < 3 ? 0: count;}
 private:
  int32_t elementID{0};
};

struct SpecialGameConfig {
  void setElementId(int32_t id) { elementID = id;}
  int32_t getElementID() { return elementID;}
  void addConfig(int32_t count, int32_t times) {
    cfg.push_back(std::pair<int32_t, int32_t> (count, times));
  }
  int32_t getConfig(int32_t count) {
    for (auto &item: cfg) {
      if (item.first == count) return item.second;
    }
    return 0;
  }
 private:
  int32_t elementID{0};
  std::vector<std::pair<int32_t, int32_t>> cfg;
};

class TSConfig {
public:
  void setRowNumber(int32_t row) {maxRow = row;}
  void setColumnNumber(int32_t col) {maxColumn = col;}
  int32_t getRowNumber() {return maxRow;}
  int32_t getColumnNumber() {return maxColumn;}

  TSGrid &getGrid(int32_t gridIdx, bool freeGame) {return freeGame? freeGameGrids[gridIdx]: grids[gridIdx];}
  int32_t getGridIndex(int32_t row, int32_t col) {return row * maxColumn + col;}
  TSGrid &getGrid(int32_t row, int32_t col, bool freeGame) {return getGrid(getGridIndex(row, col), freeGame);}

  bool checkGrids() {
    for (int32_t i = 0; i < maxRow; ++i) {
      for (int32_t j = 0; j < maxColumn; ++j) {
        auto index = getGridIndex(i, j);
        if(grids.find(index) == grids.end()) {
          return false;
        }
      }
    }
    return true;
  }

  void addLine(int32_t lineID, const std::vector<int32_t> &points) {
    TSLine line;
    line.setID(lineID);
    line.addPoints(points);
    lines.push_back(line);
  }
  TSLine &getLine(int32_t index) {return lines[index];}
  int32_t getLinesCount() {return lines.size();}

  void addElement(int32_t eleID, int32_t type, bool repeatable) {elements[eleID].setType(type);elements[eleID].setRepeatable(repeatable);}
  void setElementType(int32_t eleID, int32_t type) {elements[eleID].setType(type);}
  int32_t getElementType(int32_t eleID) {return elements[eleID].getType();}
  bool elementRepeatable(int32_t eleID) {return elements[eleID].repeatable();}
  void setElementRatio(int32_t eleID, int32_t lineCount, int32_t ratio) {elements[eleID].setRatio(lineCount, ratio);}
  int32_t getElementRatio(int32_t eleID, int32_t lineCount) {
    auto itr = elements.find(eleID);
    if (itr != elements.end()) {
      return itr->second.getRatio(lineCount);
    }
    return 0;
  }

  SpecialGameConfig &getFreeGameConfig() {return freeGameConfig;}
  TinyGameConfig &getTinyGameConfig() {return tinyGameConfig;}
  Butterfly &getButterfly() {return butterfly;}

  int32_t setMegawin(int32_t mWin) {megawin = mWin;}
  bool isMegawin(int32_t dest) { return (dest >= megawin);}
  int32_t setBigwin(int32_t mWin) {bigwin = mWin;}
  bool isBigwin(int32_t dest) { return (dest >= bigwin);}
  int32_t setSuperwin(int32_t swin) {superwin = swin;}
  bool isSuperwin(int32_t dest) {return (dest >= superwin);}

  void setJackpot1Limit(int32_t limit) { jackpot1Limit = limit;}
  bool enableRollJackpot1(int64_t val) {return val >= jackpot1Limit;}
  int32_t getJackpot1Limit() {return jackpot1Limit;}
  void setJackpot1(int32_t eleID, int32_t count) {jackpot1ID = eleID;jackpot1Count = count;}
  void setJackpot1EleID(int32_t eleID) {jackpot1ID = eleID;}
  void setJackpot1Count(int32_t count) {jackpot1Count = count;}
  int32_t getJackpot1EleID() {return jackpot1ID;}
  bool isJackpot1(int32_t dest, int32_t count) {return (dest == jackpot1ID && count == jackpot1Count);}

  void setJackpot2EleID(int32_t eleID) {jackpot2ID = eleID;}
  void setJackpot2Count(int32_t count) {jackpot2Count = count;}
  void setJackpot2Limit(int32_t limit) {jackpot2Limit = limit;}
  bool enableRollJackpot2(int64_t val) {return jackpot2Limit <= val;}
  int32_t getJackpot2Limit() {return jackpot2Limit;}
  void setJackpot2(int32_t eleID, int32_t count) {jackpot2ID = eleID;jackpot2Count = count;}
  int32_t getJackpot2EleID() {return jackpot2ID;}
  bool isJackpot2(int32_t dest, int32_t count) {return (dest == jackpot2ID && count == jackpot2Count);}

  void setForceWinHallPrize(int32_t val) {forceWinHallPrize = val;}
  bool isForceWinHallPrize(int32_t factor) {return factor == forceWinHallPrize;}
  void setMinHallPrizePool(int32_t val) {minHallPrizePool = val;}
  int32_t getMinHallPrizePool() {return minHallPrizePool;}
  void setTax4Hall(float val) {tax4Hall = val;}
  int64_t getTax4Hall(int64_t val) { return val * tax4Hall;}

  void setRoomReserveTime(int32_t val) {roomReserveTime = val;}
  int32_t getRoomReserveTime() {return roomReserveTime;}
  void setForceWinRoomPrize(int32_t val) {forceWinRoomPrize = val;}
  bool isForceWinRoomPrize(int32_t factor) {return factor == forceWinRoomPrize;}
  void setMinRoomPrizePool(int32_t val) {minRoomPrizePool = val;}
  int32_t getMinRoomPrizePool() {return minRoomPrizePool;}
  void setTax4Room(float val) {tax4Room = val;}
  int64_t getTax4Room(int64_t val) { return val * tax4Room;}

 private:
  std::map<int32_t, TSGrid> grids;
  std::map<int32_t, TSGrid> freeGameGrids;
  std::vector<TSLine> lines; // key: lineID, val: lines
  std::map<int32_t, TSElementRatio> elements; // key: elment id

  SpecialGameConfig freeGameConfig;
  TinyGameConfig tinyGameConfig;
  Butterfly butterfly;

  int32_t megawin{0};
  int32_t bigwin{0};
  int32_t superwin{0};
  /* jackpot1: room*/
  int32_t jackpot1Limit{5000};
  int32_t jackpot1ID{0};
  int32_t jackpot1Count{100};
  /* jackpot2: hall*/
  int32_t jackpot2Limit{10000};
  int32_t jackpot2ID{0};
  int32_t jackpot2Count{100};

  int32_t roomReserveTime{600};
  int32_t forceWinHallPrize{10000};
  int32_t minHallPrizePool{100000};
  float tax4Hall{0.01};
  int32_t forceWinRoomPrize{10000};
  int32_t minRoomPrizePool{50000};
  float tax4Room{0.05};

  int32_t maxRow{0};
  int32_t maxColumn{0};
  bool bEleRepeatInCol{false};
};

END_NAMESPACE
#endif
