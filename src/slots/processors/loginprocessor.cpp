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
    auto &gHistory = context.user->gDetail;
    auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
    int64_t yesterday = thisMorning - 24 * 3600;
    auto &lastLogin = gHistory.lastLogin;
    auto &loginDays = gHistory.consitiveLogin;
    if (lastLogin < yesterday) {
        loginDays = 1;
        context.events.push_back(EventInfo(EGE_LOGIN, loginDays));
        processReward(loginDays, context.user->uRes.vipLevel, context.user->loginReward);
    } else if (lastLogin >= yesterday && lastLogin < thisMorning) {
        ++loginDays;
        context.events.push_back(EventInfo(EGE_LOGIN, loginDays));
        processReward(loginDays, context.user->uRes.vipLevel, context.user->loginReward);
    }
    CLOG(INFO) << "User " << gHistory.uid << " login. Days:" << loginDays;
    lastLogin = now;
    gHistory.changed = true;
    return true;
}

void LoginProcessor::processReward(
    int32_t loginDays, int32_t level, LoginReward &loginReward) const
{
    int32_t dayn = loginDays % 7;
    if (dayn == 0) {
        dayn = 7;
    }
    auto itr1 = _config.loginDaysBonus.find(dayn);
    if (itr1 == _config.loginDaysBonus.end()){
        loginReward.setDaysReward(0);
    } else {
        loginReward.setDaysReward(itr1->second);
    }
    auto itr = _config.levelBonus.find(level);
    if (itr == _config.levelBonus.end()) {
        loginReward.setSpecialReward(0);
    } else {
        loginReward.setSpecialReward(loginReward.daysReward * (itr->second/100.0));
    }
    srand((int)time(0));
    int32_t val = rand() % CHANCE_MAX_POINT;
    int64_t bonus = 0;
    for (auto &item: _config.runnerBonus) {
        if (val < item.second) {
            bonus = item.first;
            break;
        }
    }
    loginReward.setRunnerReward(bonus);
    loginReward.setRecved(false);
}

END_NAMESPACE


