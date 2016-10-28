#ifndef FREEGAMECONFIG_H
#define FREEGAMECONFIG_H

#include <tuple>
#include "jsonconfigbase.h"

BEGIN_NAMESPACE(slots)

class FreeGameConfig: public JsonConfigBase{
public:
  FreeGameConfig();
  ~FreeGameConfig();

  int32_t begin();
  int32_t end();
  const std::pair<int32_t, int32_t> &getItem(int index);

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  std::vector<std::pair<int32_t, int32_t> > _cfg;// pair<count, times>
};

END_NAMESPACE
#endif
