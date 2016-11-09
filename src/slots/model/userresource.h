#ifndef USERRESOURCE_H
#define USERRESOURCE_H

#include "tablebase.h"
#include "util/common_define.h"
#include "util/stringutil.h"
#include "mysql/mysqlsimpleupdate.h"

BEGIN_NAMESPACE(slots)

namespace UserResourceStr{
  const std::string sTableName = "user_resource";
  const std::string sUid = "uid";
  const std::string sLevel = "level";
  const std::string sExp = "exp";
  const std::string sFortune = "fortune";
  const std::string sVipLevel = "vip_level";
  const std::string sVipPoint = "vip_point";
};

struct UserResource{
    int32_t level{0};
    int64_t exp{0};
    int64_t fortune{0};
    MutableField<int32_t> vipLevel;
    MutableField<int64_t> vipPoint;
    MutableField<int32_t> tmpVipLevel;
    MutableField<int64_t> tmpVipEndTime;

#define EZ_INT(target)                          \
    cgserver::StringUtil::toString(target)

    bool deserialize(const std::vector<std::string> &vec) {
        if (vec.size() < 8) return false;
        bool ret = cgserver::StringUtil::StrToInt32(vec[1].data(), level);
        ret = ret && cgserver::StringUtil::StrToInt64(vec[2].data(), exp);
        ret = ret && cgserver::StringUtil::StrToInt64(vec[3].data(), fortune);
        ret = ret && cgserver::StringUtil::StrToInt32(vec[4].data(), vipLevel.val);
        ret = ret && cgserver::StringUtil::StrToInt64(vec[5].data(), vipPoint.val);
        return ret;
    }

    std::string updateAll(const std::string &uid) {
        cgserver::MysqlSimpleUpdate msu;
        msu.setTable(UserResourceStr::sTableName);
        msu.addUpdateValue(UserResourceStr::sLevel, EZ_INT(level), false);
        msu.addUpdateValue(UserResourceStr::sExp, EZ_INT(exp), false);
        msu.addUpdateValue(UserResourceStr::sFortune, EZ_INT(fortune), false);
        if (vipLevel.changed) {
            msu.addUpdateValue(
                UserResourceStr::sVipLevel, EZ_INT(vipLevel.val), false);
        }
        if (vipPoint.changed) {
            msu.addUpdateValue(
                UserResourceStr::sVipPoint, EZ_INT(vipPoint.val), false);
        }
        msu.setCondition(UserResourceStr::sUid, uid, false);
        return msu.getQuery();
    }
#undef EZ_INT
    void reset() {
	level = 0;
	exp = 0;
	fortune = 0;
	vipLevel.val = 0;
	vipPoint.val = 0;
	tmpVipLevel.val = 0;
	tmpVipEndTime.val = 0;
        resetFieldStatus();
    }

    void resetFieldStatus() {
        vipLevel.changed = false;
        vipPoint.changed =false;
	tmpVipLevel.changed = false;
	tmpVipEndTime.changed = false;
    }

    void incrExp(int64_t input) {
	if (input <= 0) return;
	exp += input;
    }

    void incrVipPoint(int64_t input) {
	if (input == 0) return;
	vipPoint += input;
    }

    void levelUp() {
	level += 1;
    }

    void vipLevelUp(){
	vipLevel += 1;
    }

    void incrFortune(int64_t earned) {
	if (earned == 0)
	    return;
	fortune += earned;
	if (fortune < 0)
            fortune = 0;
    }
};
DF_SHARED_PTR(UserResource);

END_NAMESPACE
#endif
