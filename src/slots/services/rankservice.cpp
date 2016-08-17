#include "rankservice.h"
#include <slots/data/slotsdatacenter.h>

using namespace cgserver;
namespace slots{
    RankService::RankService(){
    }
    
    RankService::~RankService(){
    }

    bool RankService::doJob(CPacket &packet, CResponse &resp) {
	std::string rTypeStr;
	GET_PARAM("type", rTypeStr, true);
	uint32_t iType;
	if (!StringUtil::StrToUInt32(rTypeStr.c_str(), iType) || iType > ERT_END) {
	    CLOG(WARNING) << "Invalid type:" << rTypeStr;
	    return false;
	}
	RankType rType = RankType(iType);	
	int64_t uid;
	if (!getIntVal(packet, "uid", uid)) {
	    return false;
	}
	uint32_t page, pageSize;
	if (!getIntVal(packet, "page", page)) {
	    page = 0;
	}
	if (!getIntVal(packet, "pagesize", pageSize)) {
	    pageSize = 10;
	}
	pageSize = pageSize > 50 ? 50 : pageSize;
	
	auto &dc = SlotsDataCenter::instance();
	auto &rank = dc.getLeaderBoardData(rType);
	auto rankValue = getRankValue(rank, uid);
	SBuf buf;
	ResultFormatter rf(buf);
	rf.formatRankResult(rank, rankValue);
	resp.setBody(buf.GetString());
	return true;
    }

    int32_t RankService::getRankValue(LeaderBoardRank &rank, int64_t id){
	size_t idx = 1;
	for (auto &item: rank.data) {
	    if (item->uid == id) {
		return idx;
	    }
	    ++idx;
	}
	return RANK_LIMIT + 1;
    }
}
