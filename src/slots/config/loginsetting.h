#ifndef LOGINSETTING_H
#define LOGINSETTING_H

#include "jsonconfigbase.h"
#include "util/random.h"

BEGIN_NAMESPACE(slots)

struct RunnerConfigItem{
  int32_t id;
  int32_t weight;
  int64_t reward;
  RunnerConfigItem(){}
 RunnerConfigItem(int32_t x, int32_t y, int64_t z):
  id(x), weight(y), reward(z) {}
};

class LoginSetting: public JsonConfigBase{
public:
  LoginSetting();
  ~LoginSetting();

  int64_t getDayBonus(int32_t dayN);
  const RunnerConfigItem &getRunnerBonus();

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  std::map<int32_t, int32_t> _dayReward;
  std::vector<RunnerConfigItem> _runnerReward; // pair(reward, weight)
  cgserver::Random &_random;
  int32_t _totalWeight{0};
};

END_NAMESPACE
#endif
