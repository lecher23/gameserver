#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <slots/data/basicdata.h>
#include <set>

BEGIN_NAMESPACE(slots)

class GameContext{
 public:
    GameContext() {}
    ~GameContext() {}

    SlotsUserPtr user;
    SingleGameDetail gameInfo;    
    int64_t gameID;

    // in future, we can keep all game event here
    // game event can take affect to user status
    // GameEvents events;
    std::set<GameEvent> events;
};

END_NAMESPACE
#endif
