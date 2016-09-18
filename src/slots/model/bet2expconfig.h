#ifndef BET2EXPCONFIG_H
#define BET2EXPCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

typedef std::map<int64_t, int64_t> Bet2ExpConfigs;

class Bet2ExpConfig{
public:
  static bool deserialize(const std::vector<std::string> &row, Bet2ExpConfigs &out) {
    if (row.size() < 2) {
      CLOG(INFO) << "Invalid col num";
      return false;
    }
    int64_t key;
    bool ret = cgserver::StringUtil::StrToInt64(row[0].c_str(), key);
    int64_t val;
    ret = ret && cgserver::StringUtil::StrToInt64(row[1].c_str(), val);
    if (!ret) {
      CLOG(ERROR) << "Init level config failed.";
    }
    out[key] = val;
    return ret;
  }
};

END_NAMESPACE
#endif
