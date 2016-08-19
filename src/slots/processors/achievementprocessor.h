#ifndef ACHIEVEMENTPROCESSOR_H
#define ACHIEVEMENTPROCESSOR_H

#include <slots/data/basicdata.h>
#include <slots/data/gamecontext.h>

BEGIN_NAMESPACE(slots)

class AchievementProcessor{
 public:
    AchievementProcessor();
    ~AchievementProcessor();

    bool process(GameContext &context) const;

 private:

    void processLevel(GameContext &context) const;    
    void processFriends(GameContext &context) const;    
    void processMoney(GameContext &context) const;    
    void processGame(GameContext &context) const;    
};

END_NAMESPACE
#endif
