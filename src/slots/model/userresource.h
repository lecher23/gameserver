#ifndef USERRESOURCE_H
#define USERRESOURCE_H

#include "tablebase.h"
#include <util/common_define.h>
#include <util/stringutil.h>

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
  std::string uid;
  MutableField<int32_t> level;
  MutableField<int64_t> exp;
  MutableField<int64_t> fortune;
  MutableField<int32_t> vipLevel;
  MutableField<int64_t> vipPoint;
  MutableField<int32_t> tmpVipLevel;
  MutableField<int64_t> tmpVipEndTime;

    bool deserialize(const std::vector<std::string> &vec) {
        if (vec.size() < 8) return false;
        bool ret = cgserver::StringUtil::StrToInt32(vec[1].data(), level.val);
        ret = ret && cgserver::StringUtil::StrToInt64(vec[2].data(), exp.val);
        ret = ret && cgserver::StringUtil::StrToInt64(vec[3].data(), fortune.val);
        ret = ret && cgserver::StringUtil::StrToInt32(vec[4].data(), vipLevel.val);
        ret = ret && cgserver::StringUtil::StrToInt64(vec[5].data(), vipPoint.val);
        return ret;
    }

    void reset() {
	level.val = 1;
	exp.val = 0;
	fortune.val = 0;
	vipLevel.val = 0;
	vipPoint.val = 0;
	tmpVipLevel.val = 0;
	tmpVipEndTime.val = 0;
        resetFieldStatus();
    }

    void resetFieldStatus() {
        level.changed = false;
        exp.changed = false;
        fortune.changed = false;
        vipLevel.changed = false;
        vipPoint.changed =false;
	tmpVipLevel.changed = false;
	tmpVipEndTime.changed = false;
        _changed = false;
    }

    void incrExp(int64_t input) {
	if (input <= 0) return;
	exp += input;
        _changed =true;
    }

    void incrVipPoint(int64_t input) {
	if (input == 0) return;
	vipPoint += input;
        _changed =true;
    }

    void levelUp() {
	level += 1;
        _changed =true;
    }

    void vipLevelUp(){
	vipLevel += 1;
        _changed =true;
    }

    void incrFortune(int64_t earned) {
	if (earned == 0)
	    return;
	fortune += earned;
	if (fortune.val < 0) fortune.val = 0;
        _changed =true;
    }

    bool changed() {
        return _changed;
    }

private:
  bool _changed{false};
};
DF_SHARED_PTR(UserResource);

END_NAMESPACE
#endif
