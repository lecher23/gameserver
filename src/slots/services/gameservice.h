#ifndef GAMESERVICE_H
#define GAMESERVICE_H
#include <slots/processors/allprocessors.h>

#include "iservice.h"

namespace slots{
    class GameService:IService{
    public:
      static const std::string sTotalBet;
      static const std::string sLineNumber;
        GameService();
        virtual ~GameService();

	virtual bool doJob(CPacket &packet, CResponse &resp);

    private:
	bool doTemplePrincess(CPacket &packet, ResultFormatter &rf);
	bool doMultiple(CPacket &packet);

        GameProcessor _gProcessor;
        GameResultProcessor _hProcessor;
        AchievementProcessor _aProcessor;
    };
    const std::string GameService::sTotalBet = "b";
    const std::string GameService::sLineNumber = "l";
}
#endif
