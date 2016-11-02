#include "gameresultprocessor.h"
#include <slots/config/slotsconfig.h>
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

GameResultProcessor::GameResultProcessor(){
}

GameResultProcessor::~GameResultProcessor(){
}

bool GameResultProcessor::process(GameContext &context) const {
    auto &gInfo = context.gameInfo;
    processHall(context, gInfo);
    processGameDetail(context, gInfo);
    processLines(context, gInfo);
    // is free to play
    if (!gInfo.bFreeGame) {
	context.events.push_back(EventInfo(ECT_BET_SUM, gInfo.bet));
    } else {
        // reduce freeGameTimes
        --gInfo.freeGameTimes;
    }
    // update user fortune
    processMoney(context, gInfo);
    // update user exp&level
    processExp(context, gInfo);
    // save this result to history
    context.preGameInfo = gInfo;
    return true;
}

void GameResultProcessor::processHall(GameContext &context, GameResult &data) const {
    auto &hall = SlotsDataCenter::instance().hallOperator;
    auto &cfg = SlotsConfig::getInstance().themeConfig.tsConfig;
    auto curHallPrize = hall.incrHallPrize(context.hallID, cfg.getTax4Hall(data.bet));
    auto curHallGameCount = hall.incrHallGameCount(context.hallID);
    context.room.incrSpin();
    context.room.incrPrize(cfg.getTax4Room(data.bet));
    if (!data.bJackpot2 && cfg.isForceWinHallPrize(curHallGameCount)) {
        data.bJackpot2 = true;
        data.earned.hallPrize = curHallPrize;
        hall.incrHallPrize(context.hallID, cfg.getMinHallPrizePool()-curHallPrize);
    }
    if (!data.bJackpot1 && cfg.isForceWinRoomPrize(context.room.spinCount)) {
        data.bJackpot1 = true;
        data.earned.roomPrize = context.room.totalPrize;
        context.room.takePrize(cfg.getMinRoomPrizePool());
    }
    hall.updateRoomResource(context.hallID, context.room);
}

#define INCR_TAG_VALUE(mEvent, value)                                   \
    context.events.push_back(                                           \
        EventInfo(mEvent, TO_GAME_CJ_VALUE(gType, value)));


void GameResultProcessor::processGameDetail(
    GameContext &context, GameResult &data) const
{
    auto &tHis = context.tHis;
    auto gType = data.gType;
    // incr game times
    
    INCR_TAG_VALUE(ECT_GAME_TIMES, ++tHis.spinCount);
    if(data.bBigwin) {
        INCR_TAG_VALUE(ECT_BIGWIN, ++tHis.bigWinCount);
    }
    if(data.bMegawin) {
        INCR_TAG_VALUE(ECT_MEGAWIN, ++tHis.megaWinCount);
    }
    if(data.bSuperwin) {
        INCR_TAG_VALUE(ECT_SUPERWIN, ++tHis.superWinCount);
    }
    // bug: if both jackpot trigger, it will jump one number.
    if(data.bJackpot1) {
        INCR_TAG_VALUE(ECT_JACKPOT, ++context.gHis.jackpot);
    }
    if(data.bJackpot2) {
        INCR_TAG_VALUE(ECT_JACKPOT, ++context.gHis.jackpot);
    }
    if (data.tinyGame.enable) {
        INCR_TAG_VALUE(ECT_TINY_GAME, ++tHis.tinyGameCount);
    }
    if (data.bFreeGame) {
        ++tHis.freeGameCount;
    }
}

void GameResultProcessor::processLines(GameContext &context, GameResult &data) const {
    auto pre = context.tHis.maxLinkCount;
    for (auto &line: data.lines){
        if (line.count == 5) {
            ++context.tHis.maxLinkCount;
        }
    }
    if (pre == context.tHis.maxLinkCount)
        return ;
    context.events.push_back(
        EventInfo(ECT_SIX_LINK, pre, context.tHis.maxLinkCount));
}

void GameResultProcessor::processExp(GameContext &context, GameResult &data) const {
    auto &uRes = context.uRes;
    auto &uHis = context.gHis;
    // if zero bet then exp will not change
    if (data.bFreeGame) {
	return;
    }
    auto &levelConfig = SlotsConfig::getInstance().levelConfig;
    // exp = bet;
    auto expGot = data.bet;
    uRes.incrExp(expGot);
    int64_t expNeed = 0;
    while(true) {
        expNeed = levelConfig.expNeedForNextLevel(uRes.level.val);
        if (expNeed < 0 || expNeed > uRes.exp.val ) {
            return;
        }
        uRes.levelUp();
        // get level reward.
        auto reward = levelConfig.fortuneRewardForLevel(uRes.level.val);
        uRes.incrFortune(reward);
        uHis.newFortune(uRes.fortune.val);
        context.events.push_back(EventInfo(ECT_LEVEL, uRes.level.val));
    }
}

void GameResultProcessor::processMoney(GameContext &context, GameResult &data) const {
    auto &uHis = context.gHis;
    uHis.incrBet(data.bet);
    int64_t actualEarned = data.earned.sum() - (data.bFreeGame ? 0: data.bet);
    if (actualEarned == 0) {
	return;
    }
    UserResource &uRes = context.uRes;
    uRes.incrFortune(actualEarned);
    // update max fortune
    uHis.newFortune(uRes.fortune.val);
    // update max earned
    uHis.newEarned(data.earned.normal);
    // update earned (include this week and total)
    auto pre = uHis.totalEarned;
    uHis.incrEarned(data.earned.sum());
    if (pre != uHis.totalEarned) {
	// if total earned changed, create event.
	context.events.push_back(EventInfo(ECT_FORTUNE_SUM, pre, uHis.totalEarned));
    }
}

END_NAMESPACE
