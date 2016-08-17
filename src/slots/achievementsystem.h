#ifndef ACHIEVEMENTSYSTEM_H
#define ACHIEVEMENTSYSTEM_H

#include <slots/data/slotstables.h>

BEGIN_NAMESPACE(slots)

class AchievementSystem{
 public:
    AchievementSystem();
    ~AchievementSystem();

    void levelUP(SlotsUserPtr user, SlotsEventData &data);
    void fortuneChange(const std::string &uid, int64_t before, int64_t after);
    void gameDetailChange(SlotsUserPtr user);    
};
END_NAMESPACE
#endif
