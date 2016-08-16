#include "slotsdb.h"
#include "../util/stringutil.h"
#include "adduser2mysql.h"

using namespace cgserver;

namespace slots{
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
	UserHistory &uh = su.uHis;
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
	    uh.uid = ui.uid;
	    return true;
	}
	if (!collectSlotsUser(res[0], su)) {
	    return false;
	}
	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.setTable("history");
	mss.setCondition("uid", su.uInfo.uid, true);
	if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss)) {
	    return false;
	}
	return collectUserHistory(mss.result, su.uHis);
    }

    bool SlotsDB::getUserInfoByMachineId(const std::string &mid, SlotsUser &su) const
    {
	// get logic
	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.innerJoin("user_info", "user_resource", "uid", "uid");
	mss.addCondition("mid", su.uInfo.mid, true, true);
	return getUserInfo((MysqlOperationBase *)&mss, su);
    }

    bool SlotsDB::getUserInfoByUserId(const std::string &uid, SlotsUser &su) const
    {
	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.innerJoin("user_info", "user_resource", "uid", "uid");
	mss.addCondition("user_info.uid", uid, true, true);
	return getUserInfo((MysqlOperationBase *)&mss, su);
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
	bool ret;
	if (su->uInfo.changed) {
	    ret = updateUserInfo(su->uInfo);
	}
	if (su->uRes.changed) {
	    ret = (updateUserResource(su->uRes) && ret);
	}
	if (su->uHis.changed) {
	    ret = (updateUserHistory(su->uHis) && ret);
	}
	return ret;
    }

    bool SlotsDB::updateUserResource(UserResource &ur) const {
	MysqlSimpleUpdate msu;
	msu.setTable("user_resource");
	msu.setUpdateValue("level", StringUtil::toString(ur.level));
	msu.addUpdateValue("exp", StringUtil::toString(ur.exp));
	msu.addUpdateValue("fortune", StringUtil::toString(ur.fortune));
	msu.addUpdateValue("vip_level", StringUtil::toString(ur.vipLevel));
	msu.setCondition("uid", ur.uid, true);
	if (!_pool.doMysqlOperation((MysqlOperationBase *) &msu)) {
	    return false;
	}
	// important: if update success, it must flag to unchanged.
	ur.changed = false;
	return true;
    }

    bool SlotsDB::updateUserInfo(UserInfo &ui) const {
	MysqlSimpleUpdate msu;
	msu.setTable("user_info");	
	msu.setUpdateValue("uid", ui.uid, true);
	if (!ui.fname.empty()) {
	    msu.addUpdateValue("fname", ui.fname, true);
	}
	if (!ui.avatar.empty()) {
	    msu.addUpdateValue("avatar", ui.avatar, true);
	}
	if (!ui.male.empty()) {
	    msu.addUpdateValue("male", ui.male, true);
	}
	if (!ui.country.empty()) {
	    msu.addUpdateValue("country", ui.country, true);
	}
	msu.setCondition("uid", ui.uid, true);
	if (!_pool.doMysqlOperation((MysqlOperationBase *) &msu)) {
	    return false;
	}
	ui.changed = false;
	return true;
    }

    bool SlotsDB::updateUserHistory(UserHistory &uhis) const {
	MysqlSimpleUpdate msu;
	msu.setTable("history");
	msu.setUpdateValue("max_fortune", StringUtil::toString(uhis.maxFortune));
	msu.addUpdateValue("max_earned", StringUtil::toString(uhis.maxEarned));
	msu.addUpdateValue("total_earned", StringUtil::toString(uhis.totalEarned));
	msu.addUpdateValue("tw_earned", StringUtil::toString(uhis.twEarned));
	msu.setCondition("uid", uhis.uid, true);
	if (!_pool.doMysqlOperation((MysqlOperationBase *) &msu)) {
	    return false;
	}
	uhis.changed = false;
	return true;
    }

    bool SlotsDB::searchUser(const std::string &field, const std::string &keyword,
			     uint32_t offset, uint32_t size, SlotsUsers &users)
    {
	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.innerJoin("user_info", "user_resource", "uid", "uid");
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
	mss.innerJoin("mail_info", "mails", "mail_id", "mail_id");
	mss.setCondition("mail_info.b_delete=false", false);
	mss.addCondition("mail_info.uid", uid, true, true);
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

    //user read mail. record to db
    bool SlotsDB::readMail(const std::string &uid, const std::string &mailId) {
	MysqlSimpleUpdate msu;
	msu.setTable("mail_info");
	msu.setUpdateValue("b_read", "1");
	msu.setCondition("uid", uid, true);
	msu.addCondition("mail_id", mailId, true, true);
	if (!_pool.doMysqlOperation((MysqlOperationBase *) &msu)) {
	    CLOG(WARNING) << "Set read mail for user [" << uid << "] failed.";
	    return false;
	}
    	return true;
    }

    bool SlotsDB::delMail(const std::string &uid, const std::string &mailId) {
	MysqlSimpleUpdate msu;
	msu.setTable("mail_info");
	msu.setUpdateValue("b_read", "1");
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
	mss.setTable("m_attachment");
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
	mss.setTable("m_attachment");
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
	std::string sQuery = "select * from user_info as A inner join user_resource as B on A.uid = B.uid and (A.uid in (select uid2 as uid from friends where uid1 = ";
	sQuery += uid;
	sQuery += ") or A.uid in (select uid1 as uid from friends where uid2  =";
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
	mss.setTable("f_history");
	mss.setCondition("uid", uid, true);
	if (!_pool.doMysqlOperation((MysqlOperationBase *) &mss) ||
	    mss.result.size() == 0 || mss.result[0].size() < 4)
	{
	    return false;
	}
	MysqlRow &res = mss.result[0];
	out.uid = res[0];
	cgserver::StringUtil::StrToInt32(res[1].c_str(), out.inviteCount);
	cgserver::StringUtil::StrToInt64(res[2].c_str(), out.totalReward);
	cgserver::StringUtil::StrToInt64(res[3].c_str(), out.rewardRemain);
	return true;
    }

    bool SlotsDB::updateFHistory(const std::string &uid, const std::string &key, const std::string &value) {
	MysqlSimpleUpdate msu;
	msu.setTable("f_history");
	msu.setUpdateValue(key, value, true);
	msu.setCondition("uid", uid, true);
	return _pool.doMysqlOperation((MysqlOperationBase *) &msu);
    }

    bool SlotsDB::getReward(const std::string &uid) {
	return updateFHistory(uid, "reward_remain", "0");
    }

    bool SlotsDB::collectSlotsUsers(const cgserver::MysqlRows &rows, SlotsUsers &out) const {
	for (auto itr = rows.begin(); itr != rows.end(); ++itr){
	    SlotsUserPtr su(new SlotsUser);
	    if (collectSlotsUser(*itr, *su)) {
		out.push_back(su);
	    }
	}
	return true;
    }

    bool SlotsDB::collectUserHistory(const cgserver::MysqlRows &rows, UserHistory &uh) const{
	if (rows.empty() || rows[0].size() < 9) {
	    return false;
	}
	auto &row = rows[0];
	uh.uid = row[0];
	cgserver::StringUtil::StrToInt64(row[1].c_str(), uh.maxFortune);
	cgserver::StringUtil::StrToInt64(row[2].c_str(), uh.maxEarned);
	cgserver::StringUtil::StrToInt64(row[3].c_str(), uh.totalEarned);
	cgserver::StringUtil::StrToInt64(row[4].c_str(), uh.twEarned);
	cgserver::StringUtil::StrToInt32(row[5].c_str(), uh.lwEarnedSort);
	cgserver::StringUtil::StrToInt32(row[6].c_str(), uh.lwLevelSort);
	cgserver::StringUtil::StrToInt32(row[7].c_str(), uh.lwFortuneSort);
	cgserver::StringUtil::StrToInt32(row[8].c_str(), uh.lwAchievSort);
	return true;
    }    

    bool SlotsDB::collectSlotsUser(const cgserver::MysqlRow &row, SlotsUser &su) const{
	if (row.size() < 15) {
	    return false;
	}
	UserInfo &ui = su.uInfo;
	UserResource &ur = su.uRes;
	ui.uid = row[0];
	ui.mid = row[1];
	ui.fname = row[2];
	ui.avatar = row[4];
	ui.male = row[5];
	ui.country = row[6];
	ur.uid = row[7];	
	cgserver::StringUtil::StrToInt32(row[8].c_str(), ur.level);
	cgserver::StringUtil::StrToInt64(row[9].c_str(), ur.exp);
	cgserver::StringUtil::StrToInt64(row[10].c_str(), ur.fortune);
	cgserver::StringUtil::StrToInt32(row[11].c_str(), ur.vipLevel);
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
	msd.setTable("friends");
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
	msi.setTable("friends");
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
	    if (row.size() < 5) {
		CLOG(WARNING) << "get rank item from db failed.";
		continue;
	    }
	    LeaderBoardItemPtr tmp(new LeaderBoardItem);
	    StringUtil::StrToInt64(row[1].c_str(), tmp->uid);	    
	    tmp->name = row[2];
	    StringUtil::StrToInt32(row[3].c_str(), tmp->country);
	    StringUtil::StrToInt64(row[4].c_str(), tmp->value);
	    out.data.push_back(tmp);
	}
	return true;
    }    
}
