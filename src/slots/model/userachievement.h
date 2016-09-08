#ifndef USERACHIEVEMENT_H
#define USERACHIEVEMENT_H

#include <util/common_define.h>
#include <util/stringutil.h>

BEGIN_NAMESPACE(slots)

struct UserAchievement{
    std::string uid;
    std::string aid;
    bool isRecvReward;
    int64_t progress;
    bool isGain;
    int64_t time;
    bool changed;
    int fieldsNum() {
	return 6;
    }
    void recvReward() {
        isRecvReward = true;
        changed = true;
    }

    void setProgress(int64_t in) {
        progress = in;
        changed = true;
    }

    void gain() {
        isGain = true;
        changed = true;
    }

    void setTime(int64_t in) {
        time = in;
    }

    bool deserialize(const std::vector<std::string> &row) {
        if (row.size() < 6) {
            CLOG(WARNING) << "Invalid colum number.";
            return false;
        }
        bool ret = true;
        uid = row[0];
        aid = row[1];
        isRecvReward = (row[2] != "0");
        ret = cgserver::StringUtil::StrToInt64(row[3].c_str(), progress);
        isGain = (row[4] != "0");
        ret = ret && cgserver::StringUtil::StrToInt64(row[5].c_str(), time);
        changed = false;
        if (!ret) {
            CLOG(WARNING) << "Invalid number.";
        }
        return ret;
    }
};

#define UserCJ UserAchievement
DF_SHARED_PTR(UserCJ);
typedef std::vector<UserCJPtr> UserCJs;

END_NAMESPACE
#endif
