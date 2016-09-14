#ifndef LOGINREWARDCONFIG_H
#define LOGINREWARDCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct LoginRewardConfig{
    std::map<int32_t, int64_t> levelBonus; // vip level
    std::map<int32_t, int64_t> loginDaysBonus;
    std::vector<std::pair<int64_t, int32_t> > runnerBonus;
    std::vector<int32_t> runnerIdx;
};

#define LoginSetting LoginRewardConfig

END_NAMESPACE
#endif
