#ifndef SLOTSCONFIG_H
#define SLOTSCONFIG_H

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

namespace slots{
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
	
        SlotsConfig() {}
        ~SlotsConfig() {}

	bool init(){
	    int i;
	    for (i = 1; i < 61; ++i) {
		LevelConfig lc;
		lc.level = i;
		lc.expNeed = i*139;
		lc.maxBet = i*10;
		lc.reward = i*50;
		levelConfig.push_back(lc);
	    }
	    for (i = 1; i < 11; ++i) {
		VipLevelConfig vlc;
		vlc.level = i;
		vlc.expPer = i*139;
		vlc.goldPer = i*10;
		vlc.goldNeed = i*50;
		vipLevelConfig.push_back(vlc);		
	    }
	    return true;
	}

	const LevelConfig &getLevel(int64_t exp) {
	    for (auto itr = levelConfig.begin(); itr != levelConfig.end(); ++itr) {
		if (exp < itr->expNeed) return *itr;
	    }
	    return levelConfig.back();
	}

	int64_t expNeed2LevelUp(int64_t exp) {
	    
	    for (auto itr = levelConfig.begin(); itr != levelConfig.end(); ++itr) {
		if (exp < itr->expNeed) return (itr->expNeed - exp);
	    }
	    return (levelConfig.back().expNeed - exp);
	}
	
	const VipLevelConfig &getVipLevel(int64_t vipPoint) {
	    for (auto itr = vipLevelConfig.begin(); itr != vipLevelConfig.end(); ++itr) {
		if (vipPoint < itr->goldNeed) return *itr;
	    }
	    return vipLevelConfig.back();
	}

    private:
	// 0,50,110,180, we can use binary serch
	std::vector<LevelConfig> levelConfig;
	std::vector<VipLevelConfig> vipLevelConfig;
    };
}
#endif
