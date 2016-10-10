#ifndef ACHIEVEMENTPROCESSOR_H
#define ACHIEVEMENTPROCESSOR_H

#include <slots/data/basicdata.h>
#include <slots/data/gamecontext.h>
#include <slots/config/slotsconfig.h>

BEGIN_NAMESPACE(slots)

class AchievementProcessor{
 public:
    AchievementProcessor();
    ~AchievementProcessor();

    bool process(GameContext &context) const;

 private:

    void processLevel(GameContext &context, const EventInfo &e) const;    
    void processFriends(GameContext &context, const EventInfo &e) const;    
    void processRangeCj(GameContext &context, const EventInfo &e) const;    
    void processGame(GameContext &context, const EventInfo &e) const;

    CjSettingMap &_config;
};

END_NAMESPACE
#endif
