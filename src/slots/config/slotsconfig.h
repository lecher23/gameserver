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


BEGIN_NAMESPACE(slots)

class SlotsConfig{
 public:
    ~SlotsConfig() {}

    bool init();

    static SlotsConfig &getInstance(){
	static SlotsConfig cfg;
	return cfg;
    }

    int64_t expGain(GameContext &context, int64_t resource);
    int64_t earned(GameContext &context, int64_t src);
    int64_t vipLoginReward(int32_t level, int64_t src);

    CjSettingMap cjConfig;
    LoginSetting loginCfg;
    CargoInfos cargoInfo;
    VipConfigs vipSetting;
    LevelConfig levelConfig;
    Bet2ExpConfigs bet2Exp;
    ThemeConfig themeConfig;

private:
    SlotsConfig() {}
    SlotsConfig(const SlotsConfig &);
};

END_NAMESPACE
#endif
