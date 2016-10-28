#include "slotsdb.h"
#include <util/stringutil.h>
#include <slots/sql/adduser2mysql.h>

using namespace cgserver;
BEGIN_NAMESPACE(slots)

#define SELECT_ALL_FROM_TABLE(var, table_name)                  \
    MysqlSimpleSelect var;                                      \
    var.setField("*");                                          \
    var.setTable(table_name);                                   \
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &var)) { \
	return false;                                           \
    }


SlotsDB::SlotsDB():_pool(cgserver::MysqlConnPool::getInstance()){
}

SlotsDB::~SlotsDB(){
}

SlotsDB &SlotsDB::getInstance(){
    static SlotsDB instance;
    return instance;
}

bool SlotsDB::getUserInfo(MysqlOperationBase * mob, SlotsUser &su) const {
    UserInfo &ui = su.uInfo;
    UserResource &ur = su.uRes;
    auto &uh = su.uHis;
    if (!_pool.doMysqlOperation(mob)) {
	return false;
    }
    MysqlRows &res = mob->result;
    if (res.size() == 0 || res[0].size() == 0) {
	CLOG(INFO) << "Init user[" << ui.mid << "] to db.\n";
	std::string uid;
	if (!addUser(ui.mid, ui.uid)) {
	    CLOG(WARNING) << "Set raw user info failed.\n";
	    return false;
	}
	ur.reset();
	ur.uid = ui.uid;
	uh.reset();
	//uh.uid = ui.uid;
	uh.uid = cgserver::StringUtil::StrToInt32WithDefault(ui.uid.c_str(), 0);
	return true;
    }
    if (!su.deserialize(res[0])) {
	return false;
    }
    return getGameHistory(ui.uid, uh) && getUserAchievement(ui.uid, su.uCj);
}

bool SlotsDB::getUserInfoByMachineId(const std::string &mid, SlotsUser &su) const
{
    // get logic
    su.uInfo.mid = mid;
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.innerJoin(gUserInfo, gUserResource, "uid", "uid");
    mss.addCondition("mid", mid, true, true);
    return getUserInfo((MysqlOperationBase *)&mss, su);
}

bool SlotsDB::getUserInfoByUserId(const std::string &uid, SlotsUser &su) const
{
    su.uInfo.uid = uid;
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.innerJoin(gUserInfo, gUserResource, "uid", "uid");
    mss.addCondition(gUserInfo + ".uid", uid, true, true);
    return getUserInfo((MysqlOperationBase *)&mss, su);
}

bool SlotsDB::getUserIdByMachineId(const std::string &mid, std::string &uid) const
{
    MysqlSimpleSelect mss;
    mss.setField("uid");
    mss.setTable(gUserInfo);
    mss.setCondition("mid", mid, true);
    if (_pool.doMysqlOperation((MysqlOperationBase *) &mss)
        && mss.result.size() > 0 && mss.result[0].size() > 0)
    {
        uid = mss.result[0][0];
        return true;
    }
    return false;
}

bool SlotsDB::getCargoInfo(CargoInfos &out) const {
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(gCargoInfo);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
        return false;
    }
    for (auto &row: mss.result){
        CargoInfoPtr tmp(new CargoInfo);
        if(!tmp->deserialize(row)){
            return false;
        }
        out[tmp->cargo_id] = tmp;
    }
    return true;
}

bool SlotsDB::addUser(const std::string &mid, std::string &uid) const 
{
    AddUser2Mysql job;
    job.setMid(mid);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &job)) {
	return false;
    }
    uid = job.getUid();
    return true;
}

bool SlotsDB::update(SlotsUserPtr su) const {
    // Useless
    return false;
}

bool SlotsDB::updateGameHistory(GameHistory &gh) const {
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
        GameHistoryStr::sLoingDays, StringUtil::toString(gh.loginDays));
    msu.addUpdateValue(
        GameHistoryStr::sJackpot, StringUtil::toString(gh.jackpot));
    msu.setCondition("uid", StringUtil::toString(gh.uid), false);
    CLOG(INFO) << "update history " << msu.getQuery();
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &msu)) {
        return false;
    }
    // important: if update success, it must flag to unchanged.
    // and update theme_history is required.
    gh.changed = false;
    return true;
}

bool SlotsDB::searchUser(const std::string &field, const std::string &keyword,
			 uint32_t offset, uint32_t size, SlotsUsers &users)
{
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.innerJoin(gUserInfo, gUserResource, "uid", "uid");
    mss.addCondition(field, keyword, true, true);
    mss.setLimit(offset, size);

    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	return false;
    }
    return collectSlotsUsers(mss.result, users);
}

bool SlotsDB::getUserMails(
    const MyString &uid, MyString &offset, MyString &count, UserMails &out)
{
    out.clear();
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.innerJoin(gUserMails, gMailDetail, "mail_id", "mail_id");
    mss.setCondition(gUserMails + ".b_delete=false", false);
    mss.addCondition(gUserMails + ".uid", uid, true, true);
    mss.setSortField("ctime", false);
    mss.setLimit(offset, count);
	
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	return false;
    }

    if(!collectMailInfo(mss.result, out)) {
	CLOG(WARNING) << "Transform user [" << uid << "] mails failed.";
	return false;
    }

    return true;
}

//user read mail. record to db. infact, this param is useless
bool SlotsDB::readMail(const std::string &uid, const std::string &mailId) {
    MysqlSimpleUpdate msu;
    msu.setTable(gUserMails);
    msu.setUpdateValue("b_read", "1");
    msu.setCondition("uid", uid, true);
    msu.addCondition("mail_id", mailId, true, true);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &msu)) {
	CLOG(WARNING) << "Set read mail for user [" << uid << "] failed.";
	return false;
    }
    return true;
}

// delete mail
bool SlotsDB::delMail(const std::string &uid, const std::string &mailId) {
    MysqlSimpleUpdate msu;
    msu.setTable(gUserMails);
    msu.setUpdateValue("b_delete", "1");
    msu.setCondition("uid", uid, true);
    msu.addCondition("mail_id", mailId, true, true);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &msu)) {	
	CLOG(WARNING) << "Del mail for user [" << uid << "] failed.";
	return false;
    }
    return true;
}
    
bool SlotsDB::collectMailInfo(const MysqlRows &mails, UserMails &out){
    for (auto itr = mails.begin(); itr != mails.end(); ++itr) {
	if ((*itr).size() < 12) {
	    CLOG(WARNING) << "Invalid fields num from db. Cur:" <<
		(*itr).size() << ", E:" << 12;
	    return false;
	}
	UserMailPtr tmp(new UserMail);
	tmp->mailInfo.mailId = (*itr)[5];
	tmp->mailInfo.title = (*itr)[7];
	tmp->mailInfo.content = (*itr)[8];
	StringUtil::StrToInt32((*itr)[9].c_str(), tmp->mailInfo.attachment);
	tmp->mailInfo.createTime = (*itr)[10];
	StringUtil::StrToInt32((*itr)[11].c_str(), tmp->mailInfo.keepDays);	    

	tmp->bRead = (*itr)[2] == "0" ? false:true;
	tmp->bGet = (*itr)[3] == "0" ? false:true;
	tmp->bDel = (*itr)[4] == "0" ? false:true;
	out.push_back(tmp);
    }
    return true;
}

bool SlotsDB::getAttachments(std::map<int32_t, AttachmentPtr> &attch) {
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(gAttachment);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	return false;
    }
    for (auto &ele: mss.result) {
	if (ele.size() < 3) continue;
	int32_t attid;
	if (!StringUtil::StrToInt32(ele[0].c_str(), attid)) continue;
	auto tmp = attch.find(attid);
	if (tmp == attch.end()) {
	    attch[attid] = AttachmentPtr(new Attachment);
	}
	StringUtil::StrToInt32(ele[1].c_str(), attch[attid]->type);
	StringUtil::StrToInt64(ele[2].c_str(), attch[attid]->value);	    
    }
    return true;
}
    
bool SlotsDB::getAttachment(const std::string &attchId, Attachment &attch) {
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(gAttachment);
    mss.setCondition("attid", attchId, false);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	return false;
    }
    if (mss.result.size() == 0) {
	return false;
    }
    auto &row = mss.result[0];
    StringUtil::StrToInt32(row[1].c_str(), attch.type);
    StringUtil::StrToInt64(row[2].c_str(), attch.value);
    return true;
}
    

bool SlotsDB::getFriendsList(
    const std::string &uid, uint32_t page,uint32_t pageSize, FriendsList &list)
{
    MysqlSimpleSelect mss;
    std::string sQuery = "select * from ";
    sQuery += gUserInfo;
    sQuery +=" as A inner join ";
    sQuery += gUserResource;
    sQuery += " as B on A.uid = B.uid and (A.uid in (select uid2 as uid from ";
    sQuery += gFriends;
    sQuery += " where uid1 = ";
    sQuery += uid;
    sQuery += ") or A.uid in (select uid1 as uid from ";
    sQuery += gFriends;
    sQuery += " where uid2  =";
    sQuery += uid;
    sQuery += ")) ";
    sQuery += "limit ";
    sQuery += cgserver::StringUtil::toString(page);
    sQuery += ",";
    sQuery += cgserver::StringUtil::toString(pageSize);
    mss.setQuery(sQuery);

    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	return false;
    }
    collectSlotsUsers(mss.result, list);
    return true;
}

bool SlotsDB::getInviteHistory(const std::string &uid, FHistory &out) {
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(gFriendHistory);
    mss.setCondition("uid", uid, true);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss) ||
	mss.result.size() == 0 || mss.result[0].size() < 4)
    {
	return false;
    }
    return out.deserialize(mss.result[0]);
}

bool SlotsDB::updateFHistory(const std::string &uid, const std::string &key, const std::string &value) {
    MysqlSimpleUpdate msu;
    msu.setTable(gFriendHistory);
    msu.setUpdateValue(key, value, true);
    msu.setCondition("uid", uid, true);
    return _pool.doMysqlOperation((MysqlOperationBase *) &msu);
}

bool SlotsDB::getReward(const std::string &uid) {
    return updateFHistory(uid, "reward_remain", "0");
}

bool SlotsDB::collectSlotsUsers(const cgserver::MysqlRows &rows, SlotsUsers &out) const {
    for (auto &row: rows){
	SlotsUserPtr su(new SlotsUser);
	if (su->deserialize(row)) {
	    out.push_back(su);
	}
    }
    return true;
}

bool SlotsDB::removeFriend(const std::string &uidStr, const std::string &tidStr){
    uint64_t uid;
    if (!StringUtil::StrToUInt64(uidStr.c_str(), uid))
	return false;
    uint64_t tid;
    if (!StringUtil::StrToUInt64(tidStr.c_str(), tid))
	return false;

    MysqlSimpleDelete msd;
    msd.setTable(gFriends);
    if (tid > uid){
	msd.setCondition("uid1", uidStr, true);
	msd.addCondition("uid2", tidStr, true, true);	    
    }else if (tid < uid) {
	msd.setCondition("uid1", tidStr, true);
	msd.addCondition("uid2", uidStr, true, true);	    
    }else {
	return false;
    }
    return _pool.doMysqlOperation((MysqlOperationBase *) &msd);
}

bool SlotsDB::makeFriend(const std::string &uidStr, const std::string &tidStr) {
    uint64_t uid;
    if (!StringUtil::StrToUInt64(uidStr.c_str(), uid))
	return false;
    uint64_t tid;
    if (!StringUtil::StrToUInt64(tidStr.c_str(), tid))
	return false;
    MysqlSimpleInsert msi;
    msi.setTable(gFriends);
    msi.setField("uid1");
    msi.addField("uid2");
    // bigger id in uid2
    if (tid > uid){
	msi.setValue(uidStr);
	msi.addValue(tidStr);
    }else if (tid < uid) {
	msi.setValue(tidStr);
	msi.addValue(uidStr);
    }else{
	return false;
    }
    return _pool.doMysqlOperation((MysqlOperationBase *) &msi);	
}

bool SlotsDB::getRankData(RankType rType, LeaderBoardRank &out){
    static const std::string tables[] = {"cur_fortune_rank", "cur_earned_rank",
					 "cur_acmt_rank", "cur_level_rank",
					 "lw_fortune_rank", "lw_earned_rank",
					 "lw_acmt_rank", "lw_level_rank",
					 "tw_earned_rank"};
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(tables[rType]);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	return false;
    }
    out.data.clear();
    for (auto &row: mss.result) {
        LeaderBoardItemPtr tmp(new LeaderBoardItem);
        if(tmp->deserialize(row)) {
            out.data.push_back(tmp);
        }
    }
    return true;
}

bool SlotsDB::getGameHistory(const std::string &uid, GameHistory &gd) const {
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(gGameHistory);
    mss.setCondition("uid", uid, false);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	CLOG(WARNING) << "Get game history from mysql failed.";
	return false;
    }
    // no history
    if (mss.result.empty()) {
        CLOG(WARNING) << "No game history found for user: " << uid;
	return false;
    }
    if (!gd.deserialize(mss.result[0])){
        return false;
    }
    MysqlSimpleSelect mss1;
    mss1.setField("*");
    mss1.setTable(gThemeHistory);
    mss1.setCondition("uid", uid, false);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss1)) {
	CLOG(WARNING) << "Get game history from mysql failed.";
	return false;
    }
    return gd.themeHistory.deserialize(mss1.result);
}

bool SlotsDB::getUserAchievement(const std::string &uid, Achievements &out) const {
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(gAchievement);
    //mss.innerJoin(gAchievement, gAchievementDetail, "uaid", "aiid");
    mss.setCondition(UserCJStr::sUid, uid, false);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	CLOG(WARNING) << "Get achievement from mysql failed.";
	return false;
    }
    return collectAchievements(mss.result, out);
}

bool SlotsDB::getUserAchievement(
    const std::string &uid, const std::string &cjID, UserCJ &out)
{
    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.setTable(gAchievement);
    mss.setCondition("uid", uid, false);
    mss.addCondition("uaid", cjID, true, false);
    if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	CLOG(WARNING) << "Get achievement from mysql failed.";
	return false;
    }
    if (mss.result.size() == 0) {
	return false;
    }
    return out.deserialize(mss.result[0]);
}

bool SlotsDB::collectAchievements(const MysqlRows &result, Achievements &out) const{
    for (auto &row: result) {
	UserCJ cj;
	if (cj.deserialize(row)) {
	    out.push_back(cj);
	}
    }
    return true;
}

bool SlotsDB::getThemeCommonConfig(ThemeCommonConfig &out) const {
    SELECT_ALL_FROM_TABLE(mss, gThemeCommonConfig);
    for (auto &row: mss.result) {
        ThemeCommonConfigItem item;
        if (!item.deserialize(row)) {
            return false;
        }
        out.push_back(item);
    }
    return true;
}

#undef SELECT_ALL_FROM_TABLE

END_NAMESPACE
