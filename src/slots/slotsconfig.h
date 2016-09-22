#ifndef SLOTSCONFIG_H
#define SLOTSCONFIG_H

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <util/common_define.h>
#include <slots/data/gamecontext.h>
#include <slots/themes/tsconfig.h>
#include <slots/sql/slotsdb.h>


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
    LevelConfigs levelConfig;
    Bet2ExpConfigs bet2Exp;
    TSConfig tsConfig;

private:
    bool getSlotMachineConfig();
    bool getGridConfig(GridConfigs &gc);
    bool getLinesConfig(LinesConfig &lc);
    bool getElements(SlotElements &elements, ElementsConfig &cfg);

    SlotsConfig() {}
    SlotsConfig(const SlotsConfig &);
};

END_NAMESPACE
#endif
