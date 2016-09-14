#ifndef CARGOINFO_H
#define CARGOINFO_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct CargoInfo{
  int32_t cargo_id;
  int32_t price;
  int32_t item_type;
  int32_t extra_rate;
  int32_t sales_type;
  bool display;
  int64_t item_num;
  int64_t vip_point;

  bool deserialize(std::vector<std::string> &row) {
    if (row.size() < 8) {
      return false;
    }
    bool ret  = cgserver::StringUtil::StrToInt32(row[0].c_str(), cargo_id);
    ret = ret && cgserver::StringUtil::StrToInt32(row[1].c_str(), price);
    ret = ret && cgserver::StringUtil::StrToInt32(row[2].c_str(), item_type);
    ret = ret && cgserver::StringUtil::StrToInt64(row[3].c_str(), item_num);
    ret = ret && cgserver::StringUtil::StrToInt32(row[4].c_str(), extra_rate);
    ret = ret && cgserver::StringUtil::StrToInt32(row[5].c_str(), sales_type);
    ret = ret && cgserver::StringUtil::StrToInt64(row[6].c_str(), vip_point);
    display = (row[7] == "1");
    return ret;
  }
};
DF_SHARED_PTR(CargoInfo);
typedef std::map<int32_t, CargoInfoPtr> CargoInfos;

END_NAMESPACE
#endif
