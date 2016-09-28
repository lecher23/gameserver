#ifndef GAMERESULTPROCESSOR_H
#define GAMERESULTPROCESSOR_H

#include <slots/data/basicdata.h>
#include <slots/data/gamecontext.h>

BEGIN_NAMESPACE(slots)

class GameResultProcessor{
 public:
    GameResultProcessor();
    ~GameResultProcessor();

    bool process(GameContext &context) const;
 private:
    void processHall(GameContext &context, GameResult &data) const;
    void processGameDetail(GameContext &context, GameResult &data) const;
    void processExp(GameContext &context, GameResult &data) const;
    void processMoney(GameContext &context, GameResult &data) const;
};

END_NAMESPACE
#endif
