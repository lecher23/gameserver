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
};

struct UserInfo{
    std::string uid;
    std::string mid;
    MutableField<std::string> fname;
    MutableField<std::string> avatar;
    MutableField<std::string> male;
    MutableField<std::string> country;
    void setFname(const std::string &in) {
        fname.setVal(in);
        _changed = true;
    }
    void setAvatar(const std::string &in) {
        avatar.setVal(in);
        _changed = true;
    }
    void setMale(const std::string &in) {
        male.setVal(in);
        _changed = true;
    }
    void setCountry(const std::string &in) {
        country.setVal(in);
        _changed = true;
    }

    void resetFieldStatus() {
        fname.changed = false;
        avatar.changed = false;
        male.changed = false;
        country.changed = false;
        _changed = false;
    }

    bool changed() {
        return _changed;
    }

private:
    bool _changed{false};
};
DF_SHARED_PTR(UserInfo);

END_NAMESPACE
#endif
