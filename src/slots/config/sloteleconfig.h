#ifndef SLOTELECONFIG_H
#define SLOTELECONFIG_H

#include "jsonconfigbase.h"

BEGIN_NAMESPACE(slots)

struct RawElementItem{
  int32_t id{0};
  int32_t type{0};
  bool repeatable{false};
  std::map<int32_t, int32_t> reward; // key:link number, value: rate
};

class SlotEleConfig: public JsonConfigBase{
public:
  SlotEleConfig();
  ~SlotEleConfig();

  int32_t begin();
  int32_t end();
  const RawElementItem &getElement(int index);

  int32_t wildEleID{0};
  int32_t tinyGameEleID{0};
  int32_t freeGameEleID{0};
  int32_t roomWinEleID{0};
  int32_t hallWinEleID{0};
protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  std::vector<RawElementItem> _cfg;
};

END_NAMESPACE
#endif
