#ifndef SLOTSCONFIG_H
#define SLOTSCONFIG_H

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <util/common_define.h>
#include <slots/data/gamecontext.h>
#include <slots/model/vipconfig.h>
#include <slots/model/levelconfig.h>
#include <slots/model/bet2expconfig.h>

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

    CjSettingMap cjConfig;
    LoginSetting loginCfg; // read it from mysql or other place
    CargoInfos cargoInfo;
    VipConfigs vipSetting;
    LevelConfigs levelConfig;
    Bet2ExpConfigs bet2Exp;

private:
    SlotsConfig() {}
    SlotsConfig(const SlotsConfig &);
};

END_NAMESPACE
#endif
