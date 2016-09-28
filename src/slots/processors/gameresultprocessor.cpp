#include "gameresultprocessor.h"
#include <slots/slotsconfig.h>
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

GameResultProcessor::GameResultProcessor(){
}

GameResultProcessor::~GameResultProcessor(){
}

bool GameResultProcessor::process(GameContext &context) const {
    context.events.push_back(EventInfo(EGE_PLAYED_GAME));
    processHall(context, context.gameInfo);
    processGameDetail(context, context.gameInfo);
    // is free to play
    if (!context.gameInfo.bFreeGame) {
	context.events.push_back(EventInfo(EGE_USE_BET, context.gameInfo.bet));
    }
    // update user fortune
    processMoney(context, context.gameInfo);
    // update user exp&level
    processExp(context, context.gameInfo);
    return true;
}

void GameResultProcessor::processHall(GameContext &context, GameResult &data) const {
    auto &hall = SlotsDataCenter::instance().getHall(context.hallID);
    auto &cfg = SlotsConfig::getInstance().themeConfig.tsConfig;
    hall.incrPrize(cfg.getTax4Hall(data.bet));
    hall.incrPrize(context.roomID, cfg.getTax4Hall(data.bet));
    if (!data.bJackpot2 && cfg.isForceWinHallPrize(hall.getGameCount())) {
        data.bJackpot2 = true;
        data.earned.hallPrize = hall.takeHallPrize();
    }
    if (!data.bJackpot1 && cfg.isForceWinRoomPrize(hall.getGameCount(context.roomID))) {
        data.bJackpot1 = true;
        data.earned.roomPrize = hall.takeRoomPrize(context.roomID);
    }
    hall.incrGameCount();
    hall.incrGameCount(context.roomID);
}

#define INCR_TAG_VALUE(mThemeTag, mEvent, value)                        \
    udt.incrTagValue(gType, mThemeTag, 1);                              \
    context.events.push_back(                                           \
        EventInfo(mEvent, TO_GAME_CJ_VALUE(                             \
                      gType, udt.getTagValue(gType, mThemeTag))));


void GameResultProcessor::processGameDetail(GameContext &context, GameResult &data) const {
    auto &udt = context.user->gDetail.themeHistory;
    auto gType = data.gType;
    // incr game times
    INCR_TAG_VALUE(NORMAL_GAME_TAG, EGE_GAME_COUNT, 1);
    if(data.bBigwin) {
        INCR_TAG_VALUE(BIG_WIN_TAG, EGE_BIG_WIN, 1);
    }
    if(data.bMegawin) {
        INCR_TAG_VALUE(MEGA_WIN_TAG, EGE_MEGA_WIN, 1);
    }
    if(data.bSuperwin) {
        INCR_TAG_VALUE(SUPER_WIN_TAG, EGE_SUPER_WIN, 1);
    }
    if(data.bJackpot1) {
        INCR_TAG_VALUE(JACKPOT_TAG, EGE_JACKPOT, 1);
        ++context.user->gDetail.jackpot;
    }
    if(data.bJackpot2) {
        INCR_TAG_VALUE(JACKPOT_TAG, EGE_JACKPOT, 1);
        ++context.user->gDetail.jackpot;
    }
    if (data.tinyGame.enable) {
        INCR_TAG_VALUE(TINY_GAME_TAG, EGE_TINY_GAME, 1);
    }
    context.user->gDetail.changed = true;
}

void GameResultProcessor::processExp(GameContext &context, GameResult &data) const {
    auto &uRes = context.user->uRes;
    // if zero bet then exp will not change
    if (data.bFreeGame) {
	return;
    }
    auto &slotsConfig = SlotsConfig::getInstance();
    auto expGot = slotsConfig.expGain(context, data.bet);
    uRes.incrExp(expGot);
    int64_t expNeed = 0;
    while(true) {
        expNeed = slotsConfig.expNeedForLevelUp(uRes.level);
        if (expNeed < 0 || expNeed > uRes.exp ) {
            return;
        }
        uRes.levelUp();
        context.events.push_back(EventInfo(EGE_LEVEL_UP, uRes.level));
    }
}

void GameResultProcessor::processMoney(GameContext &context, GameResult &data) const {
    UserHistory &uHis = context.user->uHis;
    uHis.incrBet(data.bet);
    int64_t actualEarned = data.earned.sum() - data.bet;
    if (actualEarned == 0) {
	return;
    }
    UserResource &uRes = context.user->uRes;
    uRes.incrFortune(actualEarned);
    // update max fortune
    uHis.newFortune(uRes.fortune);
    // update max earned
    uHis.newEarned(data.earned.normal);
    // update earned (include this week and total)
    auto pre = uHis.totalEarned;
    uHis.incrEarned(data.earned.sum());
    if (pre != uHis.totalEarned) {
	// if total earned changed, create event.
	context.events.push_back(EventInfo(EGE_EARNED_INCR, pre, uHis.totalEarned));
    }
}

END_NAMESPACE
