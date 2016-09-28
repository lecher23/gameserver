#ifndef THEMECOMMONCONFIG_H
#define THEMECOMMONCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct ThemeCommonConfigItem{
  int32_t code{0};
  int64_t value{0};
  int64_t extra{0};
  bool deserialize(std::vector<std::string> &row) {
    if (row.size() < 3) {
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[0].c_str(), code);
    ret = ret && cgserver::StringUtil::StrToInt64(row[1].c_str(), value);
    ret = ret && cgserver::StringUtil::StrToInt64(row[2].c_str(), extra);
    return ret;
  }
};

typedef std::vector<ThemeCommonConfigItem> ThemeCommonConfig;

END_NAMESPACE
#endif
