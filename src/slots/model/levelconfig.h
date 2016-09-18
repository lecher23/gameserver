#ifndef LEVELCONFIG_H
#define LEVELCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct LevelConfig{
  int32_t level;
  int64_t expNeed;
  int64_t maxBet;
  int64_t fortuneReward;

  bool deserialize(const std::vector<std::string> &row) {
    if (row.size() < 4) {
      CLOG(INFO) << "Invalid col num";
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[0].c_str(), level);
    ret = ret && cgserver::StringUtil::StrToInt64(row[1].c_str(), expNeed);
    ret = ret && cgserver::StringUtil::StrToInt64(row[2].c_str(), maxBet);
    ret = ret && cgserver::StringUtil::StrToInt64(row[3].c_str(), fortuneReward);
    if (!ret) {
      CLOG(ERROR) << "Init level config failed.";
    }
    return ret;
  }
};
DF_SHARED_PTR(LevelConfig);
typedef std::map<int32_t, LevelConfig> LevelConfigs;

END_NAMESPACE
#endif
