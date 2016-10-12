#ifndef LEVELCONFIG_H
#define LEVELCONFIG_H

#include <slots/config/jsonconfigbase.h>


BEGIN_NAMESPACE(slots)

struct LevelConfigItem {
  int32_t level{0};
  int32_t fortuneReward{0};
  int64_t expNeed{0};
  int64_t maxBet{0};
};

class LevelConfig: public JsonConfigBase{
 public:
    LevelConfig();
    ~LevelConfig();

    int64_t expNeedForNextLevel(int32_t curLevel);
    int64_t maxBetForLevel(int32_t curLevel);
    int64_t fortuneRewardForLevel(int32_t curLevel);
    const LevelConfigItem &getLevelInfo(int32_t curLevel);

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  std::map<int32_t, LevelConfigItem> config;
};

END_NAMESPACE
#endif
