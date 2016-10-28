#include "gameprocessor.h"
#include <slots/config/slotsconfig.h>
#include <slots/data/slotsdatacenter.h>
#include <slots/themes/templeprincess.h>

BEGIN_NAMESPACE(slots)

GameProcessor::GameProcessor(){
}

GameProcessor::~GameProcessor(){
}

bool GameProcessor::process(GameContext &context) const {
    // by default, we can get hall from datacenter just by hallID
    auto hallID = context.hallID;
    auto roomID = context.room.roomID;
    auto &hall = SlotsDataCenter::instance().hallOperator;
    auto &preGame = context.user->gSt;
    auto &preGameResult = preGame.getGameResult(hallID, roomID);
    auto &gameInfo = context.gameInfo;
    auto &levelConfig = SlotsConfig::getInstance().levelConfig;
    // if this time is free game
    if (preGameResult.freeGameTimes > 0) {
        gameInfo.bFreeGame = true;
        gameInfo.bet = preGameResult.bet;
        gameInfo.lineNumber = preGameResult.lineNumber;
        gameInfo.freeGameTimes = preGameResult.freeGameTimes;
    }
    auto &uRes = context.user->uRes;
    if (!gameInfo.bFreeGame &&
        (gameInfo.bet > uRes.fortune.val ||
         gameInfo.bet > levelConfig.maxBetForLevel(uRes.level.val) ||
         gameInfo.bet <= 0))
    {
        CLOG(WARNING) << "User:"<< context.user->uInfo.uid
                      << " bet validate: cur[ " << context.user->uRes.fortune.val
                      << "], bet[" << gameInfo.bet << "] failed.";
        return false;
    }
    if (!hall.useRoom(hallID, context.uid, context.room))
    {
        CLOG(WARNING) << "User:"<< context.user->uInfo.uid
                      << " use room " << roomID << "failed.";
        return false;
    }
    // generate game result
    TemplePrincess tp(SlotsConfig::getInstance().themeConfig.tsConfig);
    if (!tp.play(context.gameInfo)) {
        return false;
    }
    return true;
}

END_NAMESPACE
