#ifndef LINECONFIG_H
#define LINECONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct LineConfig {
  std::vector<int32_t> line;
  bool deserialize(std::vector<std::string> &row) {
    if (row.size() < 2) {
      CLOG(ERROR) << "Invalid col num.";
      return false;
    }
    cgserver::StringUtil::StrToIntVector(row[1], line, ',');
    return true;
  }
};

END_NAMESPACE
#endif
