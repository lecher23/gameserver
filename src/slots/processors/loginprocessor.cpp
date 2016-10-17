#include "loginprocessor.h"
#include <time.h>
#include <stdlib.h>
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

LoginProcessor::LoginProcessor():_config(SlotsConfig::getInstance().loginCfg){
}

LoginProcessor::~LoginProcessor(){
}

bool LoginProcessor::process(GameContext &context) const {
    auto &uData = SlotsDataCenter::instance().slotsUserData;
    auto &dailyReward = context.dailyReward;
    auto &uid = context.user->uInfo.uid;

    if (uData->getDailyReward(uid, dailyReward)) {
        return true;
    }
    CLOG(INFO) << "User first login:"<< uid;

    auto thisMorning = cgserver::CTimeUtil::getMorningTime();
    auto &gHistory = context.user->uHis;
    auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
    int64_t yesterday = cgserver::CTimeUtil::getYesterdayMorning();
    auto &lastLogin = gHistory.lastLogin;
    auto &loginDays = gHistory.loginDays;
    if (lastLogin < yesterday) {
        loginDays = 1;
        context.events.push_back(EventInfo(ECT_LOGIN_DAYS, loginDays));
        processReward(loginDays, context.user->uRes.vipLevel, dailyReward);
    } else if (lastLogin >= yesterday && lastLogin < thisMorning) {
        ++loginDays;
        context.events.push_back(EventInfo(ECT_LOGIN_DAYS, loginDays));
        processReward(loginDays, context.user->uRes.vipLevel, dailyReward);
    }
    uData->setDailyReward(uid, dailyReward);
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
    loginReward.dayReward = _config.getDayBonus(dayn);

    const auto &runnerBonus = _config.getRunnerBonus();
    loginReward.runnerIdx = runnerBonus.id;
    loginReward.runnerReward = runnerBonus.reward;

    const auto &vipCfgItem =
        SlotsConfig::getInstance().vipSetting.getVipConfigInfo(vipLevel);
    loginReward.vipExtra =
        (loginReward.dayReward + runnerBonus.reward) * vipCfgItem.loginExtra;

    loginReward.recved = false;
}

END_NAMESPACE
