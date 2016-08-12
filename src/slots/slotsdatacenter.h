#ifndef SLOTSDATACENTER_H
#define SLOTSDATACENTER_H

#include "slotscache.h"
#include "slotsdb.h"
#include "slotsevent.h"
#include "giftsdata.h"
#include "../util/timeutil.h"

namespace slots{
    enum LeaderBoardType{
	ELBT_FORTUNE,
	ELBT_MAX_EARNED,
	ELBT_ACHIVEMENTS,
	ELBT_LEVEL
    };

    /*This lru cache will never expired*/
    class SlotsDataCenter{
    public:
	struct LeaderBoardData{
	    std::vector<LeaderBoardItemPtr> data;
	    int64_t timestamp;
	};

	~SlotsDataCenter(){
	}

	bool init (bool needDump = true, int dumpInterval = 60){
	    _suCache.init(300, needDump, 60);
	    _gifts.reset(new GiftsData(100));
	    slotsEvent.init();
	    return _gifts->init();
	}

	static SlotsDataCenter &instance(){
	    static SlotsDataCenter cache;
	    return cache;
	}

	bool get(std::string &uid, SlotsUserPtr &out) {
	    bool ret = _suCache.get(uid, out);
	    if (!ret) {
		CLOG(INFO) << "Get user["<< uid <<"] info from db.";
		SlotsDB &db = SlotsDB::getInstance();
		out.reset(new SlotsUser);
		std::string err;
		ret = db.getUserInfoByUserId(uid, *out);
		if (ret) {
		    out->uInfo.changed = false;
		    out->uRes.changed = false;
		    set(out);
		} else {
		    CLOG(WARNING) << "Get user[" << uid << "] info failed.";
		}
	    }else {
		ret = (out.get() != NULL);
		CLOG(INFO) << "Get user["<< uid <<"] info from cache.";
	    }
	    return ret;
	}

	bool set(SlotsUserPtr &input) {
	    return _suCache.set(input->uInfo.uid, input);
	}

	GiftsDataPtr getGiftsData() {
	    return _gifts;
	}

	void release(){
	    _suCache.stop_cache();
	}

	LeaderBoardData &getLeaderBoardData(LeaderBoardType ldBdTp) {
	    LeaderBoardData &boardData = (ldBdTp == ELBT_FORTUNE?_fortuneOrder:
					  (ldBdTp == ELBT_MAX_EARNED?_maxSingleEarned:
					   (ldBdTp == ELBT_ACHIVEMENTS?_achiveOrder:_levelOrder)));
	    if (!leaderBoardDataExpired(boardData.timestamp)) {
		return boardData;
	    }
	    refreshLeaderData(boardData, ldBdTp);
	    return boardData;
	}

	SlotsEvent slotsEvent;
    private:
	bool leaderBoardDataExpired(int64_t ts) {
	    int64_t now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
	    // just cache for 4 min.
	    return (now - ts) > 240;
	}

	void refreshLeaderData(LeaderBoardData &boardData, LeaderBoardType ldBdTp) {
	    boardData.timestamp = cgserver::CTimeUtil::getCurrentTimeInSeconds();
	    //TODO
	}
	
	GiftsDataPtr _gifts;
	
	SlotsDataCenter(){}
	SlotsDataCenter(const SlotsDataCenter &);
	
	SlotsCache<std::string, SlotsUserPtr> _suCache;

	LeaderBoardData _fortuneOrder;
	LeaderBoardData _maxSingleEarned;
	LeaderBoardData _achiveOrder;
	LeaderBoardData _levelOrder;

    };
}
#endif
