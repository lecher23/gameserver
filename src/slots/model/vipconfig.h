#ifndef VIPCONFIG_H
#define VIPCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct VipConfigItem{
  int32_t level;
  float exp_ext;
  float bounus_ext;
  float login_ext;
  int32_t exp_need;
  int32_t max_bet;
  bool deserialize(std::vector<std::string> &row) {
    if (row.size() < 6) {
      CLOG(INFO) << "Invalid col num";
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[0].c_str(), level);
    int32_t tmp;
    ret = ret && cgserver::StringUtil::StrToInt32(row[1].c_str(), tmp);
    exp_ext = tmp / 100.0;
    ret = ret && cgserver::StringUtil::StrToInt32(row[2].c_str(), tmp);
    bounus_ext = tmp / 100.0;
    ret = ret && cgserver::StringUtil::StrToInt32(row[3].c_str(), tmp);
    login_ext = tmp / 100.0;
    ret = ret && cgserver::StringUtil::StrToInt32(row[4].c_str(), exp_need);
    ret = ret && cgserver::StringUtil::StrToInt32(row[5].c_str(), max_bet);
    return ret;
  }
};
DF_SHARED_PTR(VipConfigItem);
typedef std::map<int32_t, VipConfigItemPtr> VipConfig;
typedef std::map<int32_t, VipConfigItem> VipConfigs;

END_NAMESPACE
#endif
