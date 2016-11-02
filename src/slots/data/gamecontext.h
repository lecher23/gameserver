#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <slots/data/basicdata.h>
#include <slots/config/cjconfig.h>
#include <slots/themes/hallbase.h>

BEGIN_NAMESPACE(slots)

class EventInfo{
 public:
    GameEvent e;
    int64_t preData; // data before event happen
    int64_t curData; // data after event happen
    EventInfo(GameEvent e_): e(e_) {}
    EventInfo(GameEvent e_, int64_t pre_, int64_t cur_)
	: e(e_), preData(pre_), curData(cur_) {}
    EventInfo(GameEvent e_, int64_t cur_)
    {
      e = e_;
      preData = cur_ - 1;
      curData = cur_;
    }
};

struct UserContext {
  int32_t hallID{0};
  std::string uid;

  UserInfo uInfo;
  UserResource uRes;
  GameHistory gHis;
  ThemeHistoryItem tHis;
  Achievements oldCj;
  LoginReward dailyReward;

  ThemeHistory allTHis;
  RoomInfo room;
  GameResultHistory preGameInfo;
  GameResult gameInfo;
  Achievements newCj;

  std::vector<EventInfo> events;
};

typedef UserContext GameContext;

END_NAMESPACE
#endif
