#ifndef THEMECONFIG_H
#define THEMECONFIG_H

#include "tsconfig.h"
#include "slots/config/prizepoolconfig.h"
#include "slots/config/freegameconfig.h"
#include "slots/config/sloteleconfig.h"
#include "slots/config/slotlinesconfig.h"
#include "slots/config/gridsconfig.h"
#include "slots/sql/slotsdb.h"

BEGIN_NAMESPACE(slots)

enum ThemeConfigCode{
  ROOM_RESERVE_TIME_CODE = 10006,
  ROOM_FORCE_WIN_CODE,
  ROOM_CFG_END = 19999,

  HALL_FORCE_WIN_CODE = 20005,
  HALL_CFG_END = 29999,

  WIN_MEGA_CODE = 50001,
  WIN_BIG_CODE = 50002,
  WIN_SUPER_CODE = 50003,
  WIN_CFG_END= 59999
};

class ThemeConfig{
public:
  ThemeConfig();
  ~ThemeConfig();

  bool initConfig();

  TSConfig tsConfig;

private:
    bool getTemplePrincessConfig();
    bool getGridConfig(GridsConfig &gc, bool isFree);
    bool getLinesConfig(SlotLinesConfig &lc);
    bool getElements(SlotEleConfig &cfg);
    bool getFreeGameConfig(FreeGameConfig &cfg);
    bool getPrizePoolConfig(PrizePoolConfig &cfg);
    bool getCommonConfig(ThemeCommonConfig &cfg);
};

END_NAMESPACE
#endif
