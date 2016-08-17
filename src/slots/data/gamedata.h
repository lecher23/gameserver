#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <slots/data/slotstables.h>

BEGIN_NAMESPACE(slots)

class GameData: public PersistenceBase{
 public:
    GameData();
    virtual ~GameData();
    virtual void save2MySQL();

    void initFromDb();
    GameSceneDataPtr get(int64_t uid);
    
 private:
    //std::map<GameType, GameSceneDataSet> _data;
    GameSceneDataSet _data;    
};

DF_SHARED_PTR(GameData);

END_NAMESPACE
#endif
