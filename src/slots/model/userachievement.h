#ifndef USERACHIEVEMENT_H
#define USERACHIEVEMENT_H

#include <util/common_define.h>
#include <util/stringutil.h>

BEGIN_NAMESPACE(slots)

namespace UserCJStr{
    const std::string sTableName = "user_achievement";
    const std::string sUid = "uid";
    const std::string sAid = "aid";
    const std::string sRecv = "is_recv_reward";
    const std::string sTime = "time";
};

enum CJStatus {
    CJS_CHANGED,
    CJS_SAVED,
    CJS_NEW
};

struct UserAchievement{
    int32_t uid;
    int32_t aid;
    bool isRecvReward;
    CJStatus status;
    int64_t time;

    UserAchievement(const std::string &uid_, int32_t aid_, bool recv, int64_t time_) {
        cgserver::StringUtil::StrToInt32(uid_.c_str(), uid);
        aid = aid_;
        isRecvReward = recv;
        time = time_;
        status = CJS_NEW;
    }

    UserAchievement(){}

    int fieldsNum() {
	return 4;
    }

    void recvReward() {
        isRecvReward = true;
        if (status == CJS_SAVED) {
            status = CJS_CHANGED;
        }
    }

    bool deserialize(const std::vector<std::string> &row) {
        if (row.size() < fieldsNum()) {
            CLOG(WARNING) << "Invalid colum number.";
            return false;
        }
        bool ret = cgserver::StringUtil::StrToInt32(row[0].c_str(), uid);
        ret = ret && cgserver::StringUtil::StrToInt32(row[1].c_str(), aid);
        isRecvReward = (row[2] != "0");
        ret = ret && cgserver::StringUtil::StrToInt64(row[3].c_str(), time);
        status = CJS_SAVED;
        return ret;
    }
};

#define UserCJ UserAchievement
DF_SHARED_PTR(UserCJ);
typedef std::vector<UserCJPtr> UserCJs;

END_NAMESPACE
#endif
