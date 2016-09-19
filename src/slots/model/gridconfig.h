#ifndef GRIDCONFIG_H
#define GRIDCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct GridConfig{
  int32_t gridIdx;
  int32_t eleID;
  int32_t weight;

  bool deserialize(std::vector<std::string> &row) {
    if (row.size() < 4) {
      CLOG(ERROR) << 'Invalid fields num.';
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[1].c_str(), gridIdx);
    ret = ret && cgserver::StringUtil::StrToInt32(row[2].c_str(), ele_id);
    ret = ret && cgserver::StringUtil::StrToInt32(row[3].c_str(), weight);
    return ret;
  }
};

END_NAMESPACE
#endif
