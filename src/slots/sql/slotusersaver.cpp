#include "slotusersaver.h"
#include <mysql/mysqlsimpleupdate.h>
#include <mysql/mysqlsimpleinsert.h>

BEGIN_NAMESPACE(slots)
using namespace cgserver;

SlotUserSaver::SlotUserSaver(){
}

SlotUserSaver::~SlotUserSaver(){
}

void SlotUserSaver::setUser(SlotsUserPtr in) {
    user = in;
}

bool SlotUserSaver::doOperation(MYSQL *conn) {
    if (user.get() == nullptr) {
        return true;
    }
    bool ret = false;
    startTransaction(conn);
    do{
        if (!saveUserInfo(conn, user->uInfo)) break;
        if (!saveUserResource(conn, user->uRes)) break;
        if (!saveUserHistory(conn, user->uHis)) break;
        ret = true;
    }while(false);

    endTransaction(conn, ret);
    return ret;
}

bool SlotUserSaver::saveUserInfo(MYSQL *conn, UserInfo &uInfo) {
    MysqlSimpleUpdate msu;
    msu.setTable(UserInfoStr::sTableName);
    msu.addUpdateValue("fname", uInfo.fname, true);
    msu.addUpdateValue("avatar", uInfo.avatar, true);
    msu.addUpdateValue("male", uInfo.male, true);
    msu.addUpdateValue("country", uInfo.country, true);
    msu.setCondition(UserInfoStr::sUid, user->uid, true);
    bool ret = exeQuery(conn, msu.getQuery());
    return ret;
}

bool SlotUserSaver::saveUserResource(MYSQL *conn, UserResource &ur) {
    CLOG(WARNING) << "It is no used.";
    MysqlSimpleUpdate msu;
    msu.setTable(UserResourceStr::sTableName);
    msu.addUpdateValue(
        UserResourceStr::sLevel, StringUtil::toString(ur.level));
    msu.addUpdateValue(
        UserResourceStr::sExp, StringUtil::toString(ur.exp));
    msu.addUpdateValue(
        UserResourceStr::sFortune, StringUtil::toString(ur.fortune));
    if (ur.vipLevel.changed) {
        msu.addUpdateValue(
            UserResourceStr::sVipLevel, StringUtil::toString(ur.vipLevel.val));
    }
    if (ur.vipPoint.changed) {
        msu.addUpdateValue(
            UserResourceStr::sVipPoint, StringUtil::toString(ur.vipPoint.val));
    }
    // TODO:tmp vip info should be saved!
    msu.setCondition(UserResourceStr::sUid, user->uid, true);
    bool ret = exeQuery(conn, msu.getQuery());
    ur.resetFieldStatus();
    return ret;
}

bool SlotUserSaver::saveUserHistory(MYSQL *conn, GameHistory &gh) {
    auto uidStr = StringUtil::toString(gh.uid);
    MysqlSimpleUpdate msu;
    msu.setTable(GameHistoryStr::sTableName);
    msu.setUpdateValue(
        GameHistoryStr::sMaxFortune, StringUtil::toString(gh.maxFortune));
    msu.addUpdateValue(
        GameHistoryStr::sMaxEarned, StringUtil::toString(gh.maxEarned));
    msu.addUpdateValue(
        GameHistoryStr::sTotalEarned, StringUtil::toString(gh.totalEarned));
    msu.addUpdateValue(
        GameHistoryStr::sTotalBet, StringUtil::toString(gh.totalBet));
    msu.addUpdateValue(
        GameHistoryStr::sLastLogin, StringUtil::toString(gh.lastLogin));
    msu.addUpdateValue(
        GameHistoryStr::sLoginDays, StringUtil::toString(gh.loginDays));
    msu.addUpdateValue(
        GameHistoryStr::sJackpot, StringUtil::toString(gh.jackpot));
    msu.setCondition(GameHistoryStr::sPriKey, uidStr, false);
    bool ret = exeQuery(conn, msu.getQuery());
    gh.changed = !ret;
    if (!ret) {
        return false;
    }
    return saveThemeHistory(conn, uidStr, gh.themeHistory);
}

bool SlotUserSaver::saveThemeHistory(
    MYSQL *conn, const std::string &uid, ThemeHistory &tHis)
{
    auto themes = tHis.getThemeCount();
    auto tags = tHis.getTagCount();
    for (int themeIdx = 1; themeIdx < themes; ++themeIdx) {
        std::string themeIdxStr = StringUtil::toString(themeIdx);
        for (int tagIdx = 1; tagIdx < tags; ++tagIdx) {
            auto val = tHis.getTagValue(themeIdx, tagIdx);
            MysqlSimpleInsert mss;
            mss.setTable(ThemeHistoryStr::sTableName);
            mss.setField(ThemeHistoryStr::sPriKey1);
            mss.addField(ThemeHistoryStr::sPriKey2);
            mss.addField(ThemeHistoryStr::sPriKey3);
            mss.addField(ThemeHistoryStr::sValue);
            mss.setValue(uid);
            mss.addValue(themeIdxStr);
            mss.addValue(StringUtil::toString(tagIdx));
            auto valStr = StringUtil::toString(val);
            mss.addValue(valStr);
            mss.updateIfExist();
            mss.setFieldValue(ThemeHistoryStr::sValue, valStr);
            if (!exeQuery(conn, mss.getQuery())) {
                return false;
            }
        }
    }
    return true;
}
END_NAMESPACE
