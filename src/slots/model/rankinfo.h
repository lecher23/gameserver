#ifndef RANKINFO_H
#define RANKINFO_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

#define RANK_LIMIT 1000

struct LeaderBoardItem{
    int64_t uid;
    std::string name;
    int32_t country;
    int64_t value;
};
DF_SHARED_PTR(LeaderBoardItem);

struct  LeaderBoardRank {
    std::vector<LeaderBoardItemPtr> data;
    int64_t timestamp;
    LeaderBoardRank() {
	data.resize(RANK_LIMIT);
	data.clear();
	timestamp = 0;
    }
};

struct LeaderBoardData {
    LeaderBoardRank level;
    LeaderBoardRank fortune;
    LeaderBoardRank earned;
    LeaderBoardRank achievement;
};

END_NAMESPACE
#endif
