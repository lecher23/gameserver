#include "achievementprocessor.h"
#include <util/timeutil.h>
#include <slots/data/slotsdatacenter.h>

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
    // save achievement
    auto &cjQueue = SlotsDataCenter::instance().cjQueue;
    cjQueue->addCj(context.newCj);
    // TODO: add new achievement to cache
    return true;
}

void AchievementProcessor::processRangeCj(GameContext &context, const EventInfo &e) const {
    std::vector<int32_t> ret;
    _config.getCjID(e.e, context.hallID, e.preData, e.curData, ret);
    // todo: check if this cj is got from cache
    for (auto item: ret) {
        context.newCj.push_back(
            UserCJ(context.uid, item,
               false, cgserver::CTimeUtil::getCurrentTimeInSeconds()));
    }
}

#undef TAKE_ACHIEVEMENT

END_NAMESPACE
