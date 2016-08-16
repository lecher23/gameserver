#ifndef SLOTSDATACENTER_H
#define SLOTSDATACENTER_H

#include "slotscache.h"
#include "slotsdb.h"
#include "slotsevent.h"
#include "giftsdata.h"
#include "../util/timeutil.h"

namespace slots{
#define MAX_CACHE_NUMBER 4000
#define DUMP_INTERVAL 60
    
    /*This lru cache will never expired*/
    class SlotsDataCenter{
    public:
	~SlotsDataCenter(){
	}

	bool init (bool needDump = true, int dumpInterval = DUMP_INTERVAL){
	    _suCache.init(MAX_CACHE_NUMBER, needDump, dumpInterval);
	    _gifts.reset(new GiftsData(999));
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

	LeaderBoardRank &getLeaderBoardData(RankType rType) {
	    LeaderBoardRank &rank = (rType == ERT_CUR_FORTUNE?_curRank.fortune:
				     (rType == ERT_CUR_EARNED?_curRank.earned:
				      (rType == ERT_CUR_ACHIVEMENTS?_curRank.achievement:
				       (rType == ERT_CUR_LEVEL?_curRank.level:
					(rType == ERT_LW_FORTUNE?_lwRank.fortune:
					 (rType == ERT_LW_EARNED?_lwRank.earned:
					  (rType == ERT_LW_ACHIVEMENTS?_lwRank.achievement:
					   (rType == ERT_LW_LEVEL?_lwRank.level:
					    _twEarnedRank))))))));
	    if (!rankDataExpired(rank.timestamp)) {
		return rank;
	    }
	    refreshRankData(rank, rType);
	    return rank;
	}

	AttachmentPtr getAttach(int32_t attid, bool &exist) {
	    exist = true;
	    auto res = _attachInfo.find(attid);
	    if (res != _attachInfo.end()) {
		return res->second;
	    }
	    SlotsDB &db = SlotsDB::getInstance();
	    std::string strid = cgserver::StringUtil::toString(attid);
	    AttachmentPtr ret(new Attachment);
	    exist = db.getAttachment(strid, *ret);
	    if (exist) {
		_attachInfo[attid] = ret;
	    }
	    return ret;
	}

	SlotsEvent slotsEvent;
    private:
	bool rankDataExpired(int64_t ts) {
	    int64_t now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
	    // just cache for 5 min.
	    return (now - ts) > 300;
	}

	void refreshRankData(LeaderBoardRank &rankData, RankType rType) {
	    SlotsDB &db = SlotsDB::getInstance();
	    if (db.getRankData(rType, rankData)){
		rankData.timestamp = cgserver::CTimeUtil::getCurrentTimeInSeconds();
	    }
	}
	
	
	SlotsDataCenter(){}
	SlotsDataCenter(const SlotsDataCenter &);

	/* User data*/
	SlotsCache<std::string, SlotsUserPtr> _suCache;

	/* Rank data*/
	LeaderBoardData _curRank;
	LeaderBoardData _lwRank;
	LeaderBoardRank _twEarnedRank;

	/* Gifts data*/
	GiftsDataPtr _gifts;

	/* Mail data*/
	std::map<int32_t, AttachmentPtr> _attachInfo;
    };
}
#endif
