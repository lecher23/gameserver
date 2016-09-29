#ifndef TINYGAMECONFIG_H
#define TINYGAMECONFIG_H

#include <util/common_define.h>
#include <util/stringutil.h>

BEGIN_NAMESPACE(slots)

struct TinyGameConfigItem{
  int32_t cfg_tag{0};
  int32_t cfg_value{0};
  bool deserialize(std::vector<std::string> &row) {
    if(row.size() < 4) {
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[1], cfg_tag);
    ret = ret && cgserver::StringUtil::StrToInt32(row[2], cfg_value);
    return ret;
  }
};

typedef std::vector<TinyGameConfigItem> TinyGameConfig;

END_NAMESPACE
#endif
