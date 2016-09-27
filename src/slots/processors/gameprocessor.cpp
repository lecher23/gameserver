#include "gameprocessor.h"
#include <slots/slotsconfig.h>
#include <slots/themes/templeprincess.h>

BEGIN_NAMESPACE(slots)

GameProcessor::GameProcessor(){
}

GameProcessor::~GameProcessor(){
}

bool GameProcessor::process(GameContext &context) const {
    // get previous game info
    // validate game info.
    // generate game result
    TemplePrincess tp(SlotsConfig::getInstance().themeConfig.tsConfig);
    if (!tp.play(context.gameInfo)) {
        return false;
    }
    return true;
}

END_NAMESPACE
