#include "gameprocessor.h"
#include <slots/slotsconfig.h>
#include <slots/data/slotsdatacenter.h>
#include <slots/themes/templeprincess.h>

BEGIN_NAMESPACE(slots)

GameProcessor::GameProcessor(){
}

GameProcessor::~GameProcessor(){
}

bool GameProcessor::process(GameContext &context) const {
    // by default, we can get hall from datacenter just by hallID
    auto &hall = SlotsDataCenter::instance().getHall(context.hallID);
    auto &preGame = context.user->gSt;
    if (!hall.useRoom(context.uid, context.roomID))
    {
        CLOG(WARNING) << "User:"<< context.user->uInfo.uid
                      << " use room " << context.roomID << "failed.";
        return false;
    }
    auto &preGameResult = preGame.result;
    auto &gameInfo = context.gameInfo;
    // if this time is free game
    if (preGameResult.freeGameTimes > 0) {
        gameInfo.bFreeGame = true;
        gameInfo.bet = preGameResult.bet;
        gameInfo.lineNumber = preGameResult.lineNumber;
        gameInfo.freeGameTimes = preGameResult.freeGameTimes;
    }
    // update current room info.
    preGame.roomID = context.roomID;
    preGame.hallID = context.hallID;
    // generate game result
    TemplePrincess tp(SlotsConfig::getInstance().themeConfig.tsConfig);
    if (!tp.play(context.gameInfo)) {
        return false;
    }
    return true;
}

END_NAMESPACE
