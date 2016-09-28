#ifndef TSCONFIG_H
#define TSCONFIG_H

#include <util/common_define.h>
#include <slots/model/lineconfig.h>

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

  void addPoints(std::vector<int32_t> &points) {
    _points.swap(points);
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
  int32_t _type;
  /*key: line number, value: reward. */
  std::map<int32_t, int32_t> _ratio;
};

struct SpecialGameConfig {
  void setElementId(int32_t id) { elementID = id;}
  int32_t getElementID() { return elementID;}
  void addConfig(int32_t count, int32_t times) {
    elementCount.push_back(count);
    values.push_back(times);
  }
  int32_t getConfig(int32_t count) {
    for (size_t i = 0; i < elementCount.size(); ++i) {
      if (elementCount[i] == count) {
        return values[i];
      }
    }
    return 0;
  }
 private:
  int32_t elementID{0};
  std::vector<int32_t> elementCount;
  std::vector<int32_t> values;
};

class TSConfig {
public:
  void setRowNumber(int32_t row) {maxRow = row;}

  void setColumnNumber(int32_t col) {maxColumn = col;}

  int32_t getRowNumber() {return maxRow;}

  int32_t getColumnNumber() {return maxColumn;}

  TSGrid &getGrid(int32_t gridIdx, bool freeGame) {
    return freeGame? freeGameGrids[gridIdx]: grids[gridIdx];
  }

  int32_t getGridIndex(int32_t row, int32_t col) {
    return row * maxColumn + col;
  }

  TSGrid &getGrid(int32_t row, int32_t col, bool freeGame) {
    return freeGame? freeGameGrids[getGridIndex(row, col)]: grids[getGridIndex(row, col)];
  }

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

  void addLine(int32_t lineID, std::vector<int32_t> &points) {
    TSLine line;
    line.setID(lineID);
    line.addPoints(points);
    lines.push_back(line);
  }

  TSLine &getLine(int32_t index) {
    return lines[index];
  }

  int32_t getLinesCount() {
    return lines.size();
  }

  void addElement(int32_t eleID, int32_t type) {
    elements[eleID].setType(type);
  }

  void setElementType(int32_t eleID, int32_t type) {
    elements[eleID].setType(type);
  }

  int32_t getElementType(int32_t eleID) {
    return elements[eleID].getType();
  }

  void setElementRatio(int32_t eleID, int32_t lineCount, int32_t ratio) {
    elements[eleID].setRatio(lineCount, ratio);
  }

  int32_t getElementRatio(int32_t eleID, int32_t lineCount) {
    auto itr = elements.find(eleID);
    if (itr != elements.end()) {
      return itr->second.getRatio(lineCount);
    }
    return 0;
  }

  void setRepeatElementEnabled(bool val) {
    bEleRepeatInCol = val;
  }

  bool repeatElementEnabled() {
    return bEleRepeatInCol;
  }

  SpecialGameConfig &getFreeGameConfig() {return freeGameConfig;}

  SpecialGameConfig &getTinyGameConfig() {return tinyGameConfig;}

  int32_t setMegawin(int32_t mWin) {megawin = mWin;}

  bool isMegawin(int32_t dest) { return (dest >= megawin);}

  int32_t setBigwin(int32_t mWin) {bigwin = mWin;}

  bool isBigwin(int32_t dest) { return (dest >= bigwin);}

  int32_t setSuperwin(int32_t swin) {superwin = swin;}

  bool isSuperwin(int32_t dest) {return (dest >= superwin);}

  void setJackpot1Limit(int32_t limit) {
    jackpot1Limit = limit;
  }

  int32_t getJackpot1Limit() {
    return jackpot1Limit;
  }

  void setJackpot1(int32_t eleID, int32_t count) {
    jackpot1ID = eleID;
    jackpot1Count = count;
  }

  void setJackpot1EleID(int32_t eleID) {
    jackpot1ID = eleID;
  }

  void setJackpot1Count(int32_t count) {
    jackpot1Count = count;
  }

  int32_t getJackpot1EleID() {
    return jackpot1ID;
  }

  bool isJackpot1(int32_t dest, int32_t count) {
    return (dest == jackpot1ID && count == jackpot1Count);
  }

  void setJackpot2EleID(int32_t eleID) {
    jackpot2ID = eleID;
  }

  void setJackpot2Count(int32_t count) {
    jackpot2Count = count;
  }

  void setJackpot2Limit(int32_t limit) {
    jackpot2Limit = limit;
  }

  int32_t getJackpot2Limit() {
    return jackpot2Limit;
  }

  void setJackpot2(int32_t eleID, int32_t count) {
    jackpot2ID = eleID;
    jackpot2Count = count;
  }

  int32_t getJackpot2EleID() {
    return jackpot2ID;
  }

  bool isJackpot2(int32_t dest, int32_t count) {
    return (dest == jackpot2ID && count == jackpot2Count);
  }

  void setRoomReserveTime(int32_t val) {roomReserveTime = val;}

  int32_t getRoomReserveTime() {return roomReserveTime;}

  void setForceWinHallPrize(int32_t val) {forceWinHallPrize = val;}

  bool isForceWinHallPrize(int32_t factor) {return factor == forceWinHallPrize;}

  void setMinHallPrizePool(int32_t val) {minHallPrizePool = val;}

  bool getMinHallPrizePool() {return minHallPrizePool;}

  void setTax4Hall(int32_t val) {tax4Hall = val/100.0;}

  int64_t getTax4Hall(int64_t val) { return val * tax4Hall;}

  void setForceWinRoomPrize(int32_t val) {forceWinRoomPrize = val;}

  bool isForceWinRoomPrize(int32_t factor) {return factor == forceWinRoomPrize;}

  void setMinRoomPrizePool(int32_t val) {minRoomPrizePool = val;}

  bool getMinRoomPrizePool() {return minRoomPrizePool;}

  void setTax4Room(int32_t val) {tax4Room = val/100.0;}

  int64_t getTax4Room(int64_t val) { return val * tax4Room;}
private:
  std::map<int32_t, TSGrid> grids;
  std::map<int32_t, TSGrid> freeGameGrids;
  std::vector<TSLine> lines; // key: lineID, val: lines
  std::map<int32_t, TSElementRatio> elements; // key: elment id

  SpecialGameConfig freeGameConfig;
  SpecialGameConfig tinyGameConfig;;
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
  int32_t minHallPrizePool;
  float tax4Hall{0.01};
  int32_t forceWinRoomPrize{10000};
  int32_t minRoomPrizePool;
  float tax4Room{0.05};

  int32_t maxRow{0};
  int32_t maxColumn{0};
  bool bEleRepeatInCol{false};
};

END_NAMESPACE
#endif
