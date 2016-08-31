#ifndef USERACHIEVEMENT_H
#define USERACHIEVEMENT_H

#include <util/common_define.h>

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
    UserAchievement();
};

#define UserCJ UserAchievement
DF_SHARED_PTR(UserCJ);
typedef std::vector<UserCJPtr> UserCJs;

END_NAMESPACE
#endif
