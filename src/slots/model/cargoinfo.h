#ifndef CARGOINFO_H
#define CARGOINFO_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct CargoInfo{
    std::string cid;
    int64_t base;
    int64_t vip_point;
    int64_t free_extra;
    std::vector<int32_t> vip_extra;// vip0,vip1,vip2,vip3.  15 = 15%

    bool deserialize(std::vector<std::string> &row) {
        if (row.size() < 5) {
            return false;
        }
        cid = row[0];
        if (!cgserver::StringUtil::StrToInt64(row[1].c_str(), base)) {
            return false;
        }
        if (!cgserver::StringUtil::StrToInt64(row[3].c_str(), vip_point)) {
            return false;
        }
        if (!cgserver::StringUtil::StrToInt64(row[4].c_str(), free_extra)) {
            return false;
        }
        vip_extra.clear();
        cgserver::StringUtil::StrToIntVector(row[2], vip_extra, ',');
        return true;
    }
};
DF_SHARED_PTR(CargoInfo);
typedef std::map<std::string, CargoInfoPtr> CargoInfos;

END_NAMESPACE
#endif
