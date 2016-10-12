#include "loginprocessor.h"
#include <time.h>
#include <stdlib.h>

BEGIN_NAMESPACE(slots)

LoginProcessor::LoginProcessor():_config(SlotsConfig::getInstance().loginCfg){
}

LoginProcessor::~LoginProcessor(){
}

bool LoginProcessor::process(GameContext &context) const {
    auto thisMorning = cgserver::CTimeUtil::getMorningTime();
    auto &gHistory = context.user->uHis;
    auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
    int64_t yesterday = thisMorning - 24 * 3600;
    auto &lastLogin = gHistory.lastLogin;
    auto &loginDays = gHistory.loginDays;
    if (lastLogin < yesterday) {
        loginDays = 1;
        context.events.push_back(EventInfo(ECT_LOGIN_DAYS, loginDays));
        processReward(loginDays, context.user->uRes.vipLevel, context.user->loginReward);
    } else if (lastLogin >= yesterday && lastLogin < thisMorning) {
        ++loginDays;
        context.events.push_back(EventInfo(ECT_LOGIN_DAYS, loginDays));
        processReward(loginDays, context.user->uRes.vipLevel, context.user->loginReward);
    }
    CLOG(INFO) << "User " << gHistory.uid << " login. Days:" << loginDays;
    lastLogin = now;
    gHistory.changed = true;
    return true;
}

void LoginProcessor::processReward(
    int32_t loginDays, int32_t vipLevel, LoginReward &loginReward) const
{
    int32_t dayn = loginDays % 7;
    if (dayn == 0) {
        dayn = 7;
    }

    loginReward.setDaysReward(_config.getDayBonus(dayn));
    const auto &runnerBonus = _config.getRunnerBonus();
    loginReward.setRunnerReward(runnerBonus.reward, runnerBonus.id);

    const auto &vipCfgItem =
        SlotsConfig::getInstance().vipSetting.getVipConfigInfo(vipLevel);
    auto vExt =
        (loginReward.daysReward + loginReward.runnerReward) * vipCfgItem.loginExtra;
    loginReward.setSpecialReward(vExt);
    loginReward.setRecved(false);
}

END_NAMESPACE
