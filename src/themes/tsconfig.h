#ifndef TSCONFIG_H
#define TSCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct RollLimit{
  int32_t eleID;
  int32_t lineNum;
};

struct FreeGame {
  int32_t eleID;
  int32_t lineNum;
  int32_t freeTimes;
};

struct TinyGame {
  int32_t eleID;
  int32_t lineNum;
  int32_t gameType;
};

struct TSConfig{
  /* var begin with r means room config*/
  // tax from bet
  float rTax;
  int32_t rRoomReserveTime;
  int64_t rMinPoolSize;
  int64_t rMinBetToWin;
  int64_t rBet2IncrChance;
  RollLimit rRollLimit;

  /* var begin with h means hall config*/
  RollLimit hRollLimit;
  float hTax;
  int32_t hForceRoll;

  std::vector<FreeGame> freeGameConfig;
  std::vector<TinyGame> tinyGameConfig;
  int32_t megaWinEleType;

  // win spin count: key = roomID; value = spinCount
  std::map<int32_t, int32_t> rForceRoll;
  // defautl setting
  int32_t rForceRollDefault;

  bool initFromDb(){
    // we can hold all config in single mysql table by key-value style.
    return false;
  }
};

END_NAMESPACE
#endif
