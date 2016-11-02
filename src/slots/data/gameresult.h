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

struct TinyGame {
  int32_t eleID{0};
  int32_t eleCount{0};
  int32_t tinyGameID{0};
  bool enable{false};
  void reset(){
    eleID = eleCount = tinyGameID = 0;
  }
};

// TODO: struct TinyGameResult
typedef std::vector<int32_t> TinyGameResult;

struct Profit{
  int64_t normal{0};
  int64_t roomPrize{0};
  int64_t hallPrize{0};
  int64_t tinyGame{0};
  int64_t sum() {return normal + roomPrize + hallPrize;}
  void reset() {
    tinyGame = roomPrize = hallPrize = normal = 0;
  }
};

class GameResult{
public:
  int32_t gType{0};
  int32_t lineNumber{0};
  int32_t freeGameTimes{0};
  int32_t totalRatio{0};
  TinyGame tinyGame;
  TinyGameResult tinyResult;
  bool bMegawin{false};
  bool bBigwin{false};
  bool bSuperwin{false};
  bool bJackpot1{false};
  bool bJackpot2{false};
  bool bFreeGame{false};
  Profit earned;
  int64_t bet{0}; // this is total bet, line bet is bet / lineNumber
  std::vector<LineDetail> lines; // key: lineID, val:count and eleid
  std::map<int32_t, int32_t> gridsData;
};

class GameResultHistory{
public:
    int32_t lastLines{0};
    int32_t lastHallID{0};
    int32_t freeGameTimes{0};
    int64_t lastBet{0};
    int64_t tinyGameEarned{0};
    GameResultHistory &operator = (const GameResult &right) {
        lastLines = right.lineNumber;
        freeGameTimes = right.freeGameTimes;
        tinyGameEarned = right.earned.tinyGame;
        lastBet = right.bet;
        return *this;
    }
};

typedef GameResult TSResult;

END_NAMESPACE
#endif
