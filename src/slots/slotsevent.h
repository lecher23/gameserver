#ifndef SLOTSEVENT_H
#define SLOTSEVENT_H

#include "slotsconfig.h"
#include <slots/sql/slotsdb.h>
#include <slots/achievementsystem.h>
#include <slots/data/slotsdatacenter.h>
#include <slots/data/gamedata.h>

BEGIN_NAMESPACE(slots)

class SlotsEvent{
 public:
    explicit SlotsEvent();
    ~SlotsEvent();

    void playGame(SlotsUserPtr user, SlotsEventData &data);
    void processGameDetail(SlotsUserPtr user, SlotsEventData &data);
    void processExp(SlotsUserPtr user, SlotsEventData &data);
    void processMoney(SlotsUserPtr user, SlotsEventData &data);    

    void init();

    void fortuneChange(UserResource &uRes, int64_t money);
    void expChange(UserResource &uRes, int64_t money);
    void vipPointChange(UserResource &res, int64_t money);
    void vipLevelUp(UserResource &res, int64_t money);

 private:
    SlotsConfig _slotsConfig;
    GameData _gData;
    AchievementSystem _amtSys;
};
END_NAMESPACE
#endif
