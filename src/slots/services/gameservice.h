#ifndef GAMESERVICE_H
#define GAMESERVICE_H
#include <slots/processors/allprocessors.h>

#include "iservice.h"

namespace slots{
    class GameService:IService{
    public:
        GameService();
        virtual ~GameService();

	virtual bool doJob(CPacket &packet, CResponse &resp);

    private:
	bool doSlots1(CPacket &packet, ResultFormatter &rf);
	bool doMultiple(CPacket &packet);

        GameProcessor _gProcessor;
        HistoryProcessor _hProcessor;
        AchievementProcessor _aProcessor;
    };
}
#endif
