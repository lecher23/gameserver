#ifndef ONLINECONFIG_H
#define ONLINECONFIG_H

#include <slots/config/jsonconfigbase.h>

BEGIN_NAMESPACE(slots)

// normal: 1
// runner: 2
#define ONLINE_REWARD_COUNT 5
#define MAX_ONLINE_TIME 24*3600

enum OnlineRewardType{
  EORT_NORMAL = 1,
  EORT_RUNNER = 2,
  EORT_UNKNOWN
};

struct OnlineRewardItem{
  int32_t timeReq;
  int32_t type;
  int64_t value;
 OnlineRewardItem():timeReq(0),type(0),value(0){}
 OnlineRewardItem(int32_t timeReq_, int32_t type_, int64_t value_):
  timeReq(timeReq_), type(type_), value(value_){}
};

struct OnlineRunnerItem{
  int32_t id{0};
  int32_t weight{0};
  int64_t reward{0};
};

class OnlineConfig:public JsonConfigBase{
public:
  OnlineConfig();
  ~OnlineConfig();

  int32_t nextLevel(int32_t curLevel, int32_t curTime,
                    int32_t &oTimeNeed, int64_t &oReward);
protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  int64_t runnerIt();
  void insertItem(
      int32_t index, int32_t timeRequired, int32_t rewordType, int64_t rewardValue = 0);
  void addRunnerItem(int32_t id, int32_t weigth, int64_t reward);

  std::vector<OnlineRewardItem> _cfg;
  std::vector<OnlineRunnerItem> _runner;
  bool _enableLoop;
  int32_t _runnerTotalWeight{0};
};

END_NAMESPACE
#endif
