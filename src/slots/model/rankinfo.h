#ifndef RANKINFO_H
#define RANKINFO_H

#include <util/common_define.h>
#include <util/stringutil.h>

BEGIN_NAMESPACE(slots)

#define RANK_LIMIT 1000

struct LeaderBoardItem{
    // int id(auto incr) ; this id is used to keep sort, no need in program
    int64_t uid;
    std::string name;
    int32_t country;
    int64_t value;

    bool deserialize(const std::vector<std::string> &row) {
        if (row.size() < 5) {
            CLOG(WARNING) << "Invalid colum number.";
            return false;
        }
        bool ret = cgserver::StringUtil::StrToInt64(row[1].c_str(), uid);
        name = row[2];
        ret = ret && cgserver::StringUtil::StrToInt32(row[3].c_str(), country);
        ret = ret && cgserver::StringUtil::StrToInt64(row[4].c_str(), value);
        if (!ret) {
            CLOG(WARNING) << "Invalid number.";
        }
        return ret;
    }
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
