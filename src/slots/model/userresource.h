#ifndef USERRESOURCE_H
#define USERRESOURCE_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

namespace UserResourceStr{
  const std::string sTableName = "user_resource";
  const std::string sUid = "uid";
};

struct UserResource{
  std::string uid{0};
  int32_t level{0};
  int64_t exp{0};
  int64_t fortune{0};
  int32_t vipLevel{0};
  int64_t vipPoint{0};
  int32_t tmpVipLevel{0};
  int64_t tmpVipEndTime{0};
  bool changed{false};

    UserResource() {
        reset();
    }

    void reset() {
	level = 1;
	exp = 0;
	fortune = 0;
	vipLevel = 0;
	vipPoint = 0;
	tmpVipLevel = 0;
	tmpVipEndTime = 0;
	changed = false;
    }

    void incrExp(int64_t input) {
	if (input <= 0) return;
	exp += input;
	changed = true;
    }

    void incrVipPoint(int64_t input) {
	if (input == 0) return;
	vipPoint += input;
	changed = true;
    }

    void levelUp() {
	level ++;
	changed = true;
    }

    void vipLevelUp(){
	vipLevel ++;
	changed = true;
    }

    void incrFortune(int64_t earned) {
	if (earned == 0)
	    return;
	fortune += earned;
	if (fortune < 0) fortune = 0;
	changed = true;
    }
};
DF_SHARED_PTR(UserResource);

END_NAMESPACE
#endif
