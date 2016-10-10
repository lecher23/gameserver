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
    return 999;
}

END_NAMESPACE
