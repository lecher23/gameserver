#ifndef GRIDCONFIG_H
#define GRIDCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct GridConfig{
  int32_t row;
  int32_t column;
  int32_t eleID;
  int32_t weight;

  bool deserialize(std::vector<std::string> &input) {
    if (input.size() < 4) {
      CLOG(ERROR) << "Invalid fields num.";
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(input[0].c_str(), row);
    ret = ret && cgserver::StringUtil::StrToInt32(input[1].c_str(), column);
    ret = ret && cgserver::StringUtil::StrToInt32(input[2].c_str(), eleID);
    ret = ret && cgserver::StringUtil::StrToInt32(input[3].c_str(), weight);
    return ret;
  }
};

struct GridConfigs {
  std::vector<GridConfig> grids;
  int32_t columnNum;
  int32_t rowNum;
 GridConfigs():columnNum(0), rowNum(0) {}
};

END_NAMESPACE
#endif
