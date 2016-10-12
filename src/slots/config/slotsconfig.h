#ifndef SLOTSCONFIG_H
#define SLOTSCONFIG_H

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <util/common_define.h>
#include <slots/data/gamecontext.h>
#include <slots/themes/themeconfig.h>
#include <slots/sql/slotsdb.h>
#include <slots/config/levelconfig.h>
#include <slots/config/loginsetting.h>
#include <slots/config/cjconfig.h>
#include <slots/config/vipconfig.h>

BEGIN_NAMESPACE(slots)

class SlotsConfig{
 public:
    ~SlotsConfig() {}

    bool init();

    static SlotsConfig &getInstance(){
	static SlotsConfig cfg;
	return cfg;
    }

    CjConfig cjConfig;
    LoginSetting loginCfg;
    CargoInfos cargoInfo;
    VipConfig vipSetting;
    LevelConfig levelConfig;
    Bet2ExpConfigs bet2Exp;
    ThemeConfig themeConfig;

private:
    SlotsConfig() {}
    SlotsConfig(const SlotsConfig &);
};

END_NAMESPACE
#endif
