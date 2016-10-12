#include "levelconfig.h"

BEGIN_NAMESPACE(slots)

LevelConfig::LevelConfig(){
}

LevelConfig::~LevelConfig(){
}

bool LevelConfig::parseJsonDoc(rapidjson::Document &doc) {
    for (int i = 0; i < doc.Size(); ++i) {
        rapidjson::Value &item = doc[(unsigned int) i];
        if (!item.HasMember("ID") || !item.HasMember("Exp")
            ||!item.HasMember("ChipMax")||!item.HasMember("Reward"))
        {
            CLOG(ERROR) << "Lack required key.";
            return false;
        }
        int32_t level = item["ID"].GetInt();
        config[level].level = level;
        config[level].expNeed = item["Exp"].GetInt64();
        config[level].maxBet = item["ChipMax"].GetInt64();
        config[level].fortuneReward = item["Reward"].GetInt();
    }
    return true;
}

int64_t LevelConfig::expNeedForNextLevel(int32_t curLevel) {
    auto itr = config.find(curLevel);
    if (itr != config.end()) {
        return itr->second.expNeed;
    }
    return 0;
}

int64_t LevelConfig::maxBetForLevel(int32_t curLevel) {
    auto itr = config.find(curLevel);
    if (itr != config.end()) {
        return itr->second.maxBet;
    }
    return 0;
}

int64_t LevelConfig::fortuneRewardForLevel(int32_t curLevel) {
    auto itr = config.find(curLevel);
    if (itr != config.end()) {
        return itr->second.fortuneReward;
    }
    return 0;
}

const LevelConfigItem &LevelConfig::getLevelInfo(int32_t curLevel) {
    static const LevelConfigItem defaultConfigItem;
    auto itr = config.find(curLevel);
    if (itr != config.end()) {
        return itr->second;
    }
    return defaultConfigItem;
}

END_NAMESPACE
