#ifndef LEVELCONFIG_H
#define LEVELCONFIG_H

#include <slots/config/jsonconfigbase.h>


BEGIN_NAMESPACE(slots)

struct LevelConfigItem {
  int32_t level;
  int32_t fortuneReward;
  int64_t expNeed;
  int64_t maxBet;
};

class LevelConfig: public JsonConfigBase{
 public:
    LevelConfig();
    ~LevelConfig();

    int64_t expNeedForNextLevel(int32_t curLevel);

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  std::map<int32_t, LevelConfigItem> config;
};

END_NAMESPACE
#endif
