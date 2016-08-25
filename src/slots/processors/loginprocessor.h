#ifndef LOGINPROCESSOR_H
#define LOGINPROCESSOR_H

#include <util/timeutil.h>
#include <util/common_define.h>
#include <slots/data/gamecontext.h>
#include <slots/slotsconfig.h>

BEGIN_NAMESPACE(slots)

class LoginProcessor{
public:
    LoginProcessor();
    ~LoginProcessor();

    bool process(GameContext &context) const;

private:
    void processReward
            (int32_t loginDays, int32_t level, LoginReward &loginReward) const;

    LoginSetting &_config;
};

END_NAMESPACE
#endif
