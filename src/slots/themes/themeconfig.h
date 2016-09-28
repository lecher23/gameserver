#ifndef THEMECONFIG_H
#define THEMECONFIG_H

#include <slots/themes/tsconfig.h>
#include <slots/sql/slotsdb.h>

BEGIN_NAMESPACE(slots)

enum ThemeConfigCode{
  ROOM_TAX_CODE = 10001,
  ROOM_MIN_PRIZE_CODE,
  ROOM_ROLL_ELE_CODE,
  ROOM_ROLL_ELE_COUNT_CODE,
  ROOM_ENABLE_REPEAT_ELE_CODE,
  ROOM_RESERVE_TIME_CODE,
  ROOM_FORCE_WIN_CODE,
  ROOM_MIN_BET_TO_ROLL_CODE,
  ROOM_CFG_END = 19999,

  HALL_TAX_CODE = 20001,
  HALL_MIN_PRIZE_CODE,
  HALL_ROLL_ELE_CODE,
  HALL_ROLL_ELE_COUNT_CODE,
  HALL_FORCE_WIN_CODE,
  HALL_CFG_END = 29999,

  FREE_GAME_ELE_CODE = 30000,
  FREE_GAME_CFG_END = 39999,

  TINY_GAME_ELE_CODE = 40000,
  TINY_GAME_CFG_END = 49999,

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
    bool getGridConfig(GridConfigs &gc);
    bool getLinesConfig(LinesConfig &lc);
    bool getElements(SlotElements &elements, ElementsConfig &cfg);
    bool getCommonConfig(ThemeCommonConfig &cfg);
};

END_NAMESPACE
#endif
