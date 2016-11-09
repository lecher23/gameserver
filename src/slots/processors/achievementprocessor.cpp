#include "achievementprocessor.h"
#include "util/timeutil.h"
#include "util/stringutil.h"
#include "slots/data/slotsdatacenter.h"

BEGIN_NAMESPACE(slots)

AchievementProcessor::AchievementProcessor():_config(SlotsConfig::getInstance().cjConfig){
}

AchievementProcessor::~AchievementProcessor(){
}


bool AchievementProcessor::process(GameContext &context) const {
    std::vector<EventInfo> events(context.events.begin(), context.events.end());
    for (auto &item: events) {
        processRangeCj(context, item);
    }
    return true;
}

void AchievementProcessor::processRangeCj(GameContext &context, const EventInfo &e) const {
    std::vector<int32_t> ret;
    _config.getCjID(e.e, context.hallID, e.preData, e.curData, ret);
    auto &uData = *SlotsDataCenter::instance().slotsUserData;
    for (auto cjID: ret) {
        auto cjidstr = cgserver::StringUtil::toString(cjID);
        CLOG(INFO) << "user:" << context.uid << " get new cj:"<< cjidstr;
        // if cj exist, continue.
        if (uData.isAchievementExist(context.uid, cjidstr)) {
            continue;
        }
        // add cj to new cj list.
        context.newCj.push_back(
            UserCJ(cjID, false, cgserver::CTimeUtil::getCurrentTimeInSeconds()));
        // set to cache&add to db.
        uData.setAchievement(context.uid, cjidstr, false);
    }
}

#undef TAKE_ACHIEVEMENT

END_NAMESPACE
