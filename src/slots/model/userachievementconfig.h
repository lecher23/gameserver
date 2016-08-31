#ifndef USERACHIEVEMENTCONFIG_H
#define USERACHIEVEMENTCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct UserAchievementConfig{
    std::string id;
    int64_t target;
    int64_t reward;
    int32_t reward_type;
    int32_t type;
    int32_t value;
    int fieldsNum() {
	return 6;
    }
};
#define CjSetting UserAchievementConfig
DF_SHARED_PTR(CjSetting);
typedef std::vector<CjSettingPtr> CjSettings;
typedef std::map<int32_t, CjSettings> CjSettingMap;

END_NAMESPACE
#endif
