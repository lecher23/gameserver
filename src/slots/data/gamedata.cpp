#include "gamedata.h"
BEGIN_NAMESPACE(slots)

GameData::GameData(){
}

GameData::~GameData(){
}

void GameData::save2MySQL() {
    // will not save to mysql
}

// when system start, init it from db
void GameData::initFromDb() {
    // TODO
}

GameSceneDataPtr GameData::get(int64_t uid) {
    auto res = _data.find(uid);
    if (res != _data.end()) {
	return res->second;
    }
    GameSceneDataPtr gdp(new GameSceneData);
    gdp->bet = 0;
    gdp->times = 0;
    gdp->remain = 0;
    _data[uid] = gdp;
    return gdp;
}
END_NAMESPACE
