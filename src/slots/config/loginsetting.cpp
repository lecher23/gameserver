#include "loginsetting.h"

BEGIN_NAMESPACE(slots)
using namespace JsonConfigStr;

LoginSetting::LoginSetting():_random(cgserver::Random::getInstance()){
}

LoginSetting::~LoginSetting(){
}

int64_t LoginSetting::getDayBonus(int32_t dayN) {
    auto itr = _dayReward.find(dayN);
    if (itr != _dayReward.end()) {
        return itr->second;
    }
    return 0;
}

const RunnerConfigItem &LoginSetting::getRunnerBonus() {
    static const RunnerConfigItem defaultRunnerConfigItem(0,0,0);
    auto rd = _random.randGap(0, _totalWeight);
    int32_t pre = 0;
    int32_t now;
    int32_t len = _runnerReward.size();
    for (size_t i = 0; i < len; ++i) {
        now = pre + _runnerReward[i].weight;
        if (rd >=pre && rd < now){
            return _runnerReward[i];
        }
        pre = now;
    }
    return defaultRunnerConfigItem;
}

bool LoginSetting::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sDayBonus = "DaysBonus";
    static const std::string sRunnerBonus = "RunnerBonus";
    static const std::string sDate = "Date";

    rapidjson::Value::MemberIterator itr;
    GET_MEMBER_IN_DOC(doc, itr, sDayBonus, dayBounds);
    CHECK_MEMBER_IS_ARRAY(dayBounds, sDayBonus);
    for (size_t i = 0; i < dayBounds.Capacity(); ++i) {
        GET_MEMBER_IN_DOC(dayBounds[i], itr, sDate, dayValue);
        GET_MEMBER_IN_DOC(dayBounds[i], itr, sReward, rewardValue);
        _dayReward[dayValue.GetInt()] = rewardValue.GetInt();
    }

    GET_MEMBER_IN_DOC(doc, itr, sRunnerBonus, runnerBonus);
    CHECK_MEMBER_IS_ARRAY(runnerBonus, sRunnerBonus);
    _totalWeight = 0;
    for (size_t i = 0; i < runnerBonus.Capacity(); ++i) {
        GET_MEMBER_IN_DOC(runnerBonus[i], itr, sID, idValue);
        GET_MEMBER_IN_DOC(runnerBonus[i], itr, sWeight, weightValue);
        GET_MEMBER_IN_DOC(runnerBonus[i], itr, sReward, rewardValue);
        int32_t weight = weightValue.GetInt();
        _totalWeight += weight;
        _runnerReward.push_back(
            RunnerConfigItem(idValue.GetInt(), weight, rewardValue.GetInt()));
    }
    return true;
}

END_NAMESPACE
