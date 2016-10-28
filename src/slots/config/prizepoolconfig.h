#ifndef PRIZEPOOLCONFIG_H
#define PRIZEPOOLCONFIG_H

#include "jsonconfigbase.h"
#include "util/random.h"

BEGIN_NAMESPACE(slots)

struct PoolConfigItem{
  int32_t id{0};
  int32_t minBet{0};
  float tax{0};
  int64_t base{0};
};

class PrizePoolConfig: public JsonConfigBase{
public:
  PrizePoolConfig();
  ~PrizePoolConfig();

  const PoolConfigItem &getHallConfig();
  const PoolConfigItem &getRoomConfig();

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  PoolConfigItem _hallConfig;
  PoolConfigItem _roomConfig;
};

END_NAMESPACE
#endif
