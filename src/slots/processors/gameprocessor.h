#ifndef GAMEPROCESSOR_H
#define GAMEPROCESSOR_H

#include <slots/data/gamecontext.h>

BEGIN_NAMESPACE(slots)

class GameProcessor{
 public:
    GameProcessor();
    ~GameProcessor();

    bool process(GameContext &context) const;
};

END_NAMESPACE
#endif
