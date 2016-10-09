#ifndef USERINFO_H
#define USERINFO_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

namespace UserInfoStr{
    const std::string sTableName = "user_info";
    const std::string sUid = "uid";
    const std::string sMid = "mid";
};

struct UserInfo{
UserInfo():changed(false) {}
    std::string uid;
    std::string mid;
    std::string fname;
    std::string avatar;
    std::string male; // 0 or 1
    std::string country;
    bool changed;
    void setFname(const std::string &in) {
        fname = in;
        changed = true;
    }
    void setAvatar(const std::string &in) {
        avatar = in;
        changed = true;
    }
    void setMale(const std::string &in) {
        male = in;
        changed = true;
    }
    void setCountry(const std::string &in) {
        country = in;
        changed = true;
    }
};
DF_SHARED_PTR(UserInfo);

END_NAMESPACE
#endif
