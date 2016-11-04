#ifndef USERINFO_H
#define USERINFO_H

#include <util/common_define.h>
#include "tablebase.h"

BEGIN_NAMESPACE(slots)

namespace UserInfoStr{
    const std::string sTableName = "user_info";
    const std::string sCreateUserProcedure = "create_user";
    const std::string sUid = "uid";
    const std::string sMid = "mid";
    const std::string sName = "fname";
    const std::string sGender = "male";
    const std::string sCountry = "country";
};

struct UserInfo{
    std::string uid;
    std::string mid;
    std::string fname;
    std::string avatar;
    std::string male;
    std::string country;

    bool deserialize(const std::vector<std::string> &vec) {
        if (vec.size() < 6) return false;
        fname = vec[2];
        avatar = vec[3];
        male = vec[4];
        country = vec[5];
        return true;
    }
};
DF_SHARED_PTR(UserInfo);

END_NAMESPACE
#endif
