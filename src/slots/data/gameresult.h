#ifndef GAMERESULT_H
#define GAMERESULT_H

#include<util/common_define.h>

BEGIN_NAMESPACE(slots)

struct LineDetail{
  int32_t lineID;
  int32_t eleID;
  int32_t count;
 LineDetail():eleID(0),count(0){}
 LineDetail(int32_t lID, int32_t eID, int32_t cnt)
 :lineID(lID), eleID(eID), count(cnt){}
};

struct GameResult{
  int32_t gType{0};
  int32_t lineNumber{0};
  int32_t tinyGameEleID{0};
  int32_t tinyGameEleCount{0};
  int32_t freeGameTimes{0};
  int32_t totalRatio{0};
  bool bMegawin{false};
  bool bBigwin{false};
  bool bSuperwin{false};
  bool bJackpot1{false};
  bool bJackpot2{false};
  bool bFreeGame{false};
  int64_t earned{0};
  int64_t bet{0}; // total_bet = bet * lineNumber
  std::vector<LineDetail> lines; // key: lineID, val:count and eleid
  std::map<int32_t, int32_t> gridsData;
};

typedef GameResult TSResult;

END_NAMESPACE
#endif
