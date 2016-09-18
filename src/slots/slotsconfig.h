#ifndef SLOTSCONFIG_H
#define SLOTSCONFIG_H

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <util/common_define.h>
#include <slots/data/basicdata.h>
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

    int64_t expGain(int64_t resource);

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
