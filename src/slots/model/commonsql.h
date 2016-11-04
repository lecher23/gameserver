#ifndef COMMONSQL_H
#define COMMONSQL_H

#include "userresource.h"
#include "userinfo.h"

BEGIN_NAMESPACE(slots)

class CommonSQL{
 public:
    CommonSQL();
    ~CommonSQL();

    static std::string updateUserFortune(
            const std::string &uid, int64_t fortrune)
    {
        cgserver::MysqlSimpleUpdate msu;
        msu.setTable(UserResourceStr::sTableName);
        msu.addUpdateValue(
            UserResourceStr::sFortune,cgserver::StringUtil::toString(fortune), false);
        msu.setCondition(UserResourceStr::sUid, uid, true, false);
        return msu.getQuery();
    }

#define EZ_UPDATE(nameSpace, out, field, value)                 \
    cgserver::MysqlSimpleUpdate out;                            \
    out.setTable(nameSpace::sTableName);                        \
    out.addUpdateValue(nameSpace::field, value, true);          \
    msu.setCondition(nameSpace::sUid, uid, true, false);

    static std::string updateUserName(
        const std::string &uid, const std::string &name)
    {
        EZ_UPDATE_USER_INFO(UserInfoStr, msu, sName, name);
        return msu.getQuery();
    }

    static std::string updateUserSex(
        const std::string &uid, bool isMale)
    {
        EZ_UPDATE_USER_INFO(UserInfoStr, msu, sGender, isMale ? "1": "0");
        return msu.getQuery();
    }

    static std::string updateUserCountry(
        const std::string &uid, const std::string &country)
    {
        EZ_UPDATE_USER_INFO(UserInfoStr, msu, sCountry, country);
        return msu.getQuery();
    }

#undef EZ_UPDATE
};

END_NAMESPACE
#endif
