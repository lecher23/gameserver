#ifndef VIPCONFIG_H
#define VIPCONFIG_H

#include <slots/config/jsonconfigbase.h>

BEGIN_NAMESPACE(slots)

struct VipConfigItem{
  int32_t level{0};
  float expExtra{0.0};
  float bonusExtra{0.0};
  float loginExtra{0.0};
  int32_t pointNeed{0};
  int32_t maxBet{0};
};

class VipConfig: public JsonConfigBase{
public:
  VipConfig();
  ~VipConfig();

  const VipConfigItem &getVipConfigInfo(int32_t vipLevel);

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  std::map<int32_t, VipConfigItem> config;
};

END_NAMESPACE
#endif
