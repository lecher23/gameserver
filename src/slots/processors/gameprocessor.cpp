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
    auto roomID = context.roomID;
    auto &hall = SlotsDataCenter::instance().getHall(hallID);
    auto &preGame = context.user->gSt;
    auto &preGameResult = preGame.getGameResult(hallID, roomID);
    auto &gameInfo = context.gameInfo;
    // if this time is free game
    if (preGameResult.freeGameTimes > 0) {
        gameInfo.bFreeGame = true;
        gameInfo.bet = preGameResult.bet;
        gameInfo.lineNumber = preGameResult.lineNumber;
        gameInfo.freeGameTimes = preGameResult.freeGameTimes;
    }
    if (!gameInfo.bFreeGame && gameInfo.bet > context.user->uRes.fortune) {
        CLOG(WARNING) << "User:"<< context.user->uInfo.uid
                      << " has not enough money: cur[ " << context.user->uRes.fortune
                      << "], bet[" << gameInfo.bet << "]";
        return false;
    }
    if (!hall.useRoom(context.uid, roomID))
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
