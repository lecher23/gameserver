#include "gameprocessor.h"
#include <util/luatoolfactory.h>

BEGIN_NAMESPACE(slots)

GameProcessor::GameProcessor(){
}

GameProcessor::~GameProcessor(){
}

bool GameProcessor::process(GameContext &context) const {
    cgserver::LuaToolPtr lua = cgserver::LuaToolFactory::getInstance().borrowTool();
    if (lua.get() == NULL) {
	CLOG(WARNING) << "Get lua tool failed.";
	return false;
    }
    /* Input: game_type, user_level, vip_level, bet, place_holder
       Return: bet_change, detail*/
    lua->chooseFunc("main");
    lua->pushValue(int64_t(1));
    auto &uRes = context.user->uRes;
    lua->pushValue((int64_t)uRes.level);
    lua->pushValue((int64_t)uRes.vipLevel);
    lua->pushValue(context.gameInfo.bet);
    lua->pushValue("");
    if (!lua->exeFunc(5, 2)) {
	CLOG(WARNING) << "Exe lua function failed.";
	return false;
    }
    if (!lua->getValue(context.gameInfo.detail)){
	CLOG(WARNING) << "Get detail from lua script failed.";
	return false;
    }
    int64_t moneyEarned;
    if (!lua->getValue(moneyEarned) || moneyEarned < 0) {
	CLOG(WARNING) << "Invalid bet earned.";	    
	return false;
    }
    CLOG(INFO) << "User [" << context.user->uInfo.uid << "] earn:" << moneyEarned;
    return true;
}

END_NAMESPACE
