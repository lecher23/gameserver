#ifndef COMMONSQL_H
#define COMMONSQL_H

#include "userresource.h"
#include "userinfo.h"
#include "util/timeutil.h"
#include "util/stringutil.h"

BEGIN_NAMESPACE(slots)

class CommonSQL{
 public:
    CommonSQL();
    ~CommonSQL();

    static std::string updateUserFortune(
            const std::string &uid, int64_t fortune)
    {
        cgserver::MysqlSimpleUpdate msu;
        msu.setTable(UserResourceStr::sTableName);
        msu.addUpdateValue(
            UserResourceStr::sFortune,cgserver::StringUtil::toString(fortune), false);
        msu.setCondition(UserResourceStr::sUid, uid, false);
        return msu.getQuery();
    }

#define EZ_UPDATE(nameSpace, out, field, value)                 \
    cgserver::MysqlSimpleUpdate out;                            \
    out.setTable(nameSpace::sTableName);                        \
    out.addUpdateValue(nameSpace::field, value, true);          \
    out.setCondition(nameSpace::sUid, uid, false);

    static std::string updateUserName(
        const std::string &uid, const std::string &name)
    {
        EZ_UPDATE(UserInfoStr, msu, sName, name);
        return msu.getQuery();
    }

    static std::string updateUserSex(
        const std::string &uid, bool isMale)
    {
        EZ_UPDATE(UserInfoStr, msu, sGender, isMale ? "1": "0");
        return msu.getQuery();
    }

    static std::string updateUserCountry(
        const std::string &uid, const std::string &country)
    {
        EZ_UPDATE(UserInfoStr, msu, sCountry, country);
        return msu.getQuery();
    }

    static std::string updateUserAchievement(
        const std::string &uid, const std::string cjID, bool recved)
    {
        cgserver::MysqlSimpleInsert msi;
        msi.setTable(UserCJStr::sTableName);
        msi.setField(UserCJStr::sUid);
        msi.addField(UserCJStr::sAid);
        msi.addField(UserCJStr::sRecv);
        msi.addField(UserCJStr::sTime);
        msi.setValue(uid);
        msi.addValue(cjID);
        msi.addValue(recved? sMysqlTrue: sMysqlFalse);
        auto gotTime = cgserver::CTimeUtil::getCurrentTimeInSeconds();
        msi.addValue(cgserver::StringUtil::toString(gotTime));
        msi.updateIfExist();
        msi.setFieldValue(UserCJStr::sRecv, recved? sMysqlTrue: sMysqlFalse);
        return msi.getQuery();
    }

    static std::string addAchievement(
            const std::string &uid, const std::string cjID)
    {
        cgserver::MysqlSimpleInsert msi;
        msi.setTable(UserCJStr::sTableName);
        msi.setField(UserCJStr::sUid);
        msi.addField(UserCJStr::sAid);
        msi.setValue(uid);
        msi.addValue(cjID);
        return msi.getQuery();
    }
#undef EZ_UPDATE
};

END_NAMESPACE
#endif
