#ifndef THEMECONFIG_H
#define THEMECONFIG_H

#include <slots/themes/tsconfig.h>
#include <slots/sql/slotsdb.h>

BEGIN_NAMESPACE(slots)

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
};

END_NAMESPACE
#endif
