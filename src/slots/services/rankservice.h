#ifndef RANKSERVICE_H
#define RANKSERVICE_H

#include "iservice.h"

namespace slots{
    class RankService:IService{
    public:
        RankService();
        virtual ~RankService();

	virtual bool doJob(CPacket &packet, CResponse &resp);
    private:
	int32_t getRankValue(LeaderBoardRank &rank, int64_t id);	
    };
}
#endif
