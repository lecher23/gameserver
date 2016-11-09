#ifndef SLOTSDATACENTER_H
#define SLOTSDATACENTER_H

#include <slots/sql/slotsdb.h>
#include <slots/data/persistencethread.h>
#include <slots/data/slotsuserdata.h>
#include <slots/data/sqlqueue.h>
#include <slots/data/cjqueue.h>
#include <slots/data/giftsdata.h>
#include <slots/themes/hallbase.h>
#include <slots/config/slotsconfig.h>
#include <util/timeutil.h>
#include "util/producerconsumerqueue.h"
#include "backupconsumer.h"

BEGIN_NAMESPACE(slots)
#define MAX_CACHE_NUMBER 4096
#define DUMP_INTERVAL 60


/*This lru cache will never expired*/
class SlotsDataCenter{
public:
    ~SlotsDataCenter(){
    }

    bool init (bool needDump = true, int dumpInterval = DUMP_INTERVAL){
	_gifts.reset(new GiftsData(999));

	slotsUserData.reset(new SlotsUserData(backupQueue));

        auto &slotsConfig = SlotsConfig::getInstance();
        if(!slotsConfig.init()){
          CLOG(ERROR) << "Init game config failed.";
          return false;
        }
	return sqlConsumer.start() &&  _gifts->init();
    }

    static SlotsDataCenter &instance(){
	static SlotsDataCenter cache;
	return cache;
    }

    GiftsDataPtr getGiftsData() {
	return _gifts;
    }

    void release(){
        sqlConsumer.stop();
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

    /* User data*/
    cgserver::ProducerConsumerQueue<std::string> backupQueue;
    SlotsUserDataPtr slotsUserData;
    BackupConsumer sqlConsumer;

    HallBase hallOperator;

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

    SlotsDataCenter():backupQueue(),sqlConsumer(backupQueue){}
    SlotsDataCenter(const SlotsDataCenter &);

    /* Rank data*/
    LeaderBoardData _curRank;
    LeaderBoardData _lwRank;
    LeaderBoardRank _twEarnedRank;

    /* Gifts data*/
    GiftsDataPtr _gifts;

    /* Mail data*/
    std::map<int32_t, AttachmentPtr> _attachInfo;
};
END_NAMESPACE
#endif
