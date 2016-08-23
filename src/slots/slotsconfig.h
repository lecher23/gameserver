#ifndef SLOTSCONFIG_H
#define SLOTSCONFIG_H

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <util/common_define.h>
#include <slots/data/basicdata.h>

BEGIN_NAMESPACE(slots)
class SlotsConfig{
 public:
    struct LevelConfig{
	int32_t level;
	int64_t expNeed;
	int64_t maxBet;
	int64_t reward;
    };
	
    struct VipLevelConfig{
	int32_t level;
	float expPer;
	float goldPer;
	int64_t goldNeed;
    };

    ~SlotsConfig() {}

    bool init();

    const LevelConfig &getLevel(int64_t exp);
    int64_t expGain(int64_t money);
    int64_t expNeed2LevelUp(int64_t exp);
    const VipLevelConfig &getVipLevel(int64_t vipPoint);
    
    static SlotsConfig &getInstance(){
	static SlotsConfig cfg;
	return cfg;
    }

    std::vector<LevelConfig> levelConfig;
    std::vector<VipLevelConfig> vipLevelConfig;
    CjSettingMap cjConfig;
    
 private:
    SlotsConfig() {}
    SlotsConfig(const SlotsConfig &);
};
typedef SlotsConfig::LevelConfig SlotsLevelConfig;
typedef SlotsConfig::VipLevelConfig SlotsVipConfig;

END_NAMESPACE
#endif
