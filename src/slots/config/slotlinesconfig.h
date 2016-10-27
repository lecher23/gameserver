#ifndef SLOTLINESCONFIG_H
#define SLOTLINESCONFIG_H

#include "jsonconfigbase.h"

BEGIN_NAMESPACE(slots)

class SlotLinesConfig: public JsonConfigBase{
public:
  SlotLinesConfig();
  ~SlotLinesConfig();

  int32_t getPointCount();
  int32_t begin();
  int32_t end();
  const std::vector<int32_t> &getLine(int32_t index);

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  int32_t pointCount{0};
  std::vector<std::vector<int32_t> > _lines;
};

END_NAMESPACE
#endif
