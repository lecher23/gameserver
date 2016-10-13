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
    int32_t lineNumber{0};
    int32_t freeGameTimes{0};
    int32_t totalRatio{0};
    TinyGame tinyGame;
    Profit earned;
    int64_t bet{0};
    GameResultHistory &operator = (const GameResult &right) {
        lineNumber = right.lineNumber;
        freeGameTimes = right.freeGameTimes;
        totalRatio = right.totalRatio;
        tinyGame = right.tinyGame;
        earned = right.earned;
        bet = right.bet;
        return *this;
    }

    void reset() {
      totalRatio = freeGameTimes = lineNumber = 0;
      tinyGame.reset();
      earned.reset();
      bet = 0;
    }
};

typedef GameResult TSResult;

class GameStatus {
public:
  GameResultHistory &getGameResult(int32_t hall_id, int32_t room_id) {
    if (hallID != hall_id || roomID != room_id) {
      hallID = hall_id;
      roomID = room_id;
      result.reset();
    }
    return result;
  }

  int64_t tinyGameEarned() {
    if (!result.tinyGame.enable) {
      return 0;
    }
    return result.earned.tinyGame;
  }

private:
  int32_t hallID{0};
  int32_t roomID{0};
  GameResultHistory result;
};

END_NAMESPACE
#endif
