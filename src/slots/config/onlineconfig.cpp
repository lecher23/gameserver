#include "onlineconfig.h"
#include <util/random.h>

BEGIN_NAMESPACE(slots)
using namespace JsonConfigStr;

OnlineConfig::OnlineConfig(){
}

OnlineConfig::~OnlineConfig(){
}

// return: next level
int32_t OnlineConfig::nextLevel(
    int32_t curLevel, int32_t curTime, int32_t &oTimeNeed, int64_t &oReward)
{
    oReward = 0;
    if (curLevel >= _cfg.size()) {
        oTimeNeed = MAX_ONLINE_TIME;
        return curLevel;
    }
    const auto &curItem = _cfg[curLevel];
    oTimeNeed = curItem.timeReq - curTime;
    if (oTimeNeed > 0) {
        return curLevel;
    }
    if (curItem.type == EORT_RUNNER) {
        oReward = runnerIt();
    } else if (curItem.type == EORT_NORMAL) {
        oReward = curItem.value;
    }
    if (++curLevel == _cfg.size()){
        if (!_enableLoop){
            oTimeNeed = MAX_ONLINE_TIME;
            return curLevel;
        }
        curLevel = 0;
    }
    oTimeNeed = _cfg[curLevel].timeReq;
    return curLevel;
}

bool OnlineConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sOnlineReward = "OnlineReward";
    static const std::string sOnlineWheel = "OnlineWheel";
    static const std::string sOnlineEnableLoop = "EnableLoop";
    static const std::string sOnline = "Online";

    _cfg.clear();
    rapidjson::Value::MemberIterator itr;
    GET_MEMBER_IN_DOC(doc, itr, sOnlineEnableLoop, enableLoop);
    _enableLoop = enableLoop.GetBool();

    GET_MEMBER_IN_DOC(doc, itr, sOnlineReward, onlineReward);
    CHECK_MEMBER_IS_ARRAY(onlineReward, sOnlineReward);
    for (size_t i = 0; i < onlineReward.Capacity(); ++i) {
        GET_MEMBER_IN_DOC(onlineReward[i], itr, sOnline, timeReqValue);
        GET_MEMBER_IN_DOC(onlineReward[i], itr, sRewardType, rwdTypeValue);
        GET_MEMBER_IN_DOC(onlineReward[i], itr, sReward, rwdValue);
        GET_MEMBER_IN_DOC(onlineReward[i], itr, sID, idValue);
        if (rwdTypeValue.GetInt() == EORT_NORMAL) {
            insertItem(idValue.GetInt(), timeReqValue.GetInt(),
                       rwdTypeValue.GetInt(), rwdValue.GetInt64());
        }else {
            insertItem(idValue.GetInt(), timeReqValue.GetInt(),
                       rwdTypeValue.GetInt());
        }
    }

    GET_MEMBER_IN_DOC(doc, itr, sOnlineWheel, runnerBonus);
    CHECK_MEMBER_IS_ARRAY(runnerBonus, sOnlineWheel);
    for (size_t i = 0; i < runnerBonus.Capacity(); ++i) {
        GET_MEMBER_IN_DOC(runnerBonus[i], itr, sID, idValue);
        GET_MEMBER_IN_DOC(runnerBonus[i], itr, sWeight, weightValue);
        GET_MEMBER_IN_DOC(runnerBonus[i], itr, sReward, rewardValue);
        addRunnerItem(idValue.GetInt(), weightValue.GetInt(), rewardValue.GetInt64());
    }
    return true;
}

int64_t OnlineConfig::runnerIt() {
    auto wt = cgserver::Random::getInstance().randGap(0, _runnerTotalWeight);
    int32_t begin = 0;
    int32_t end = 0;
    for (auto &item: _runner) {
        end = begin + item.weight;
        if (begin <= wt && end > wt) {
            return item.reward;
        }
        begin = end;
    }
    return 0;
}

void OnlineConfig::insertItem(
    int32_t index, int32_t timeRequired, int32_t rewardType, int64_t rewardValue)
{
    _cfg.push_back(OnlineRewardItem(timeRequired, rewardType, rewardValue));
}

void OnlineConfig::addRunnerItem(int32_t id, int32_t weight, int64_t reward) {
    OnlineRunnerItem item;
    item.id = id;
    item.weight = weight;
    item.reward = reward;
    _runnerTotalWeight += weight;
    _runner.push_back(item);
}


END_NAMESPACE
