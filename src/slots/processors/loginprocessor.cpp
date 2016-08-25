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
    } else if (lastLogin >= yesterday && lastLogin < thisMorning) {
        ++loginDays;
        context.events.push_back(EventInfo(EGE_LOGIN, loginDays));
        processReward(loginDays, context.user->uRes.level, context.user->loginReward);
    }
    lastLogin = now;
    return true;
}

void LoginProcessor::processReward(
    int32_t loginDays, int32_t level, LoginReward &loginReward) const 
{
    int32_t dayn = loginDays % 7 + 1;
    auto itr = _config.levelBonus.find(level);
    if (itr == _config.levelBonus.end()){
        loginReward.specialReward = 0;
    } else {
        loginReward.specialReward = itr->second;
    }
    auto itr1 = _config.loginDaysBonus.find(loginDays);
    if (itr1 == _config.loginDaysBonus.end()){
        loginReward.daysReward = 0;
    } else {
        loginReward.daysReward = itr1->second;
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
    loginReward.runnerReward = bonus;
    loginReward.recved = false;
}

END_NAMESPACE
