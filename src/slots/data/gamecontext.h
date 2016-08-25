#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <slots/data/basicdata.h>
#include <set>

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
	: e(e_), curData(cur_) {}
};

class GameContext{
 public:
    GameContext() {}
    ~GameContext() {}

    SlotsUserPtr user;
    SingleGameDetail gameInfo;
    int64_t gameID;
    std::vector<UserCJ> userCj;

    // to save all game event during game.
    std::vector<EventInfo> events;
};

END_NAMESPACE
#endif
