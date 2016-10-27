#ifndef GRIDSCONFIG_H
#define GRIDSCONFIG_H

#include "jsonconfigbase.h"

BEGIN_NAMESPACE(slots)

struct RawGridItem{
  int32_t col{0};
  int32_t row{0};
  int32_t eleID{0};
  int32_t weight{0};
  RawGridItem(){}
 RawGridItem(int32_t x, int32_t y, int32_t z, int32_t m)
 :col(x), row(y), eleID(z), weight(m){}
};

class GridsConfig: public JsonConfigBase{
public:
  GridsConfig();
  ~GridsConfig();

  int32_t start();
  int32_t end();
  const RawGridItem &getGrid(int32_t idx);
protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
    std::vector<RawGridItem> _cfg;
    int32_t colSize{0};
    int32_t rowSize{0};
};

END_NAMESPACE
#endif
