#include "slotsdb.h"
#include "../mysql/sqlupdate.h"
#include "../mysql/sqlselect.h"
#include "../mysql/sqldelete.h"
#include "../util/stringutil.h"

using namespace cgserver;

namespace slots{
    SlotsDB::SlotsDB():_client(cgserver::MysqlClient::getInstance()){
    }
    
    SlotsDB::~SlotsDB(){
    }

    SlotsDB &SlotsDB::getInstance(){
	static SlotsDB instance;
	return instance;
    }
    
#define APPEND_VALUE(prefix, val)		\
    prefix##Query.append(1,'"');		\
    prefix##Query.append(val);			\
    prefix##Query.append(1,'"');

// only value not empty will be set.
#define SET_VALUE(prefix, key, val, seq)	\
    if (!val.empty()){				\
	prefix##Query.append(key);		\
	prefix##Query.append(1,'=');		\
	APPEND_VALUE(prefix, val);		\
	if (seq) {				\
	    prefix##Query.append(1, ',');	\
	}					\
    }

    bool SlotsDB::getUserInfo(const std::string &sQuery, SlotsUser &su) const {
	UserInfo &ui = su.uInfo;
	UserResource &ur = su.uRes;
	MysqlRow res;
	if (!_client.queryWithResult(sQuery, res)) {
	    CLOG(WARNING) << "Run query [" << sQuery << "] failed.\n";
	    //errMsg = "Run query failed.Code 110001.";
	    return false;
	}
	if (res.size() == 0) {
	    CLOG(INFO) << "Init user[" << ui.mid << "] to db.\n";
	    std::string uid;
	    if (!addUser(ui.mid, ui.uid)) {
		CLOG(WARNING) << "Set raw user info failed.\n";
		return false;
	    }
	    if (!_client.queryWithResult(sQuery, res)) {
		CLOG(WARNING) << "Run query [" << sQuery << "] failed.\n";
		return false;
	    }
	    if (res.size() == 0) {
		CLOG(WARNING) << "Init user failed.";
		return false;
	    }
	}
	return collectSlotsUser(res, su);
    }

    bool SlotsDB::getUserInfoByMachineId(const std::string &mid, SlotsUser &su) const
    {
	// get logic
	std::string sQuery = "select * from user_info as A inner join user_resource as B on A.uid = B.uid and A.mid = ";
	APPEND_VALUE(s, su.uInfo.mid);
	return getUserInfo(sQuery, su);
    }

    bool SlotsDB::getUserInfoByUserId(const std::string &uid, SlotsUser &su) const
    {
	// get logic
	std::string sQuery = "select * from user_info as A inner join user_resource as B on A.uid = B.uid and A.uid = ";
	APPEND_VALUE(s, uid);
	return getUserInfo(sQuery, su);
    }

    bool SlotsDB::addUser(const std::string &mid, std::string &uid) const 
    {
	// we should use transaction to make sure data ACID.
	// first we should forbid autocommit by using mysql_autocommit(&sql, false);
	std::string insertQuery = "insert into user_info (mid) values(";
	APPEND_VALUE(insert, mid);
	insertQuery.append(1, ')');
	
	std::string selectQuery = "select uid from user_info where mid=";
	APPEND_VALUE(select, mid);

	MysqlRow out;
	if (!_client.insertWithReturn(insertQuery, selectQuery, out) || out.empty()) {
	    CLOG(ERROR) << "Add new user failed.\n";
	    return false;
	}
	uid = out[0];
	if (!_client.addRow("user_resource", "uid", uid)) {
	    CLOG(WARNING) << "Add new user ["<< mid << "] failed.";
	    return false;
	}
	if (!_client.addRow("f_history", "uid", uid)) {
	    CLOG(WARNING) << "Add new user ["<< mid << "] failed.";
	    return false;
	}
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
	return ret;
    }

    bool SlotsDB::updateUserResource(UserResource &ur) const {
	std::string query;
	SqlUpdate su(query);
	su.setTable("user_resource");
	su.updateVal("level", ur.level);
	su.updateVal("exp", ur.exp);
	su.updateVal("fortune", ur.fortune);
	su.updateVal("vip_level", ur.vipLevel);
	su.hasCondition();
	su.addEqualCondition("uid", ur.uid);
	if (!_client.query(query)) {
	    return false;
	}
	// important: if update success, it must flag to unchanged.
	ur.changed = false;
	return true;
    }

    bool SlotsDB::updateUserInfo(UserInfo &ui) const {
	// we should use transaction to make sure data ACID.
	// first we should forbid autocommit by using mysql_autocommit(&sql, false);
	std::string updateQuery = "UPDATE user_info SET ";
	SET_VALUE(update, "fname", ui.fname, true);
	//SET_VALUE(update, "lname", ui.lname, true);
	SET_VALUE(update, "avatar", ui.avatar, true);
	SET_VALUE(update, "male", ui.male, true);
	SET_VALUE(update, "country", ui.country, true);
	SET_VALUE(update, "uid", ui.uid, false);
	updateQuery.append(" where uid = ");
	APPEND_VALUE(update, ui.uid);
	
	if (!_client.query(updateQuery)) {
	    return false;
	}
	ui.changed = false;
	return true;
    }

    bool SlotsDB::searchUser(const std::string &field, const std::string &keyword,
			     uint32_t offset, uint32_t size, SlotsUsers &users)
    {
	MysqlRows result;
	std::string q;
	cgserver::SqlSelect ss(q);
	ss.addField("*");
	ss.innerJoin("user_info", "user_resource", "uid", "uid");
	ss.setConditionJoin(true);
	ss.addEqualCondition(field, keyword);
	ss.setLimit(offset, size);
	if (!_client.queryWithResult(q, result)) {
	    return false;
	}
	return collectSlotsUsers(result, users);
    }

    bool SlotsDB::getUserMails(
	const MyString &uid, MyString &offset, MyString &count, UserMails &out)
    {
	out.clear();
	std::string sQuery = "SELECT * from mail_info as A INNER JOIN mails"
	    " as B on A.mail_id = B.mail_id WHERE A.b_delete = false and A.uid=";
	APPEND_VALUE(s, uid);
	sQuery.append(" order by ctime desc limit ");
	sQuery.append(offset);
	sQuery.append(1, ',');
	sQuery.append(count);

	MysqlRows result;
	if (!_client.queryWithResult(sQuery, result)) {
	    CLOG(WARNING) << "Get user [" << uid << "] mails from db failed.";
	    return false;
	}

	if(!getMailInfo(result, out)) {
	    CLOG(WARNING) << "Transform user [" << uid << "] mails failed.";
	    return false;
	}

	return true;
    }

    //user read mail. record to db
    bool SlotsDB::readMail(const std::string &uid, const std::string &mailId) {
	std::string query;
	SqlUpdate su(query);
	su.setTable("mail_info");
	su.updateValue("b_read", "1");
	su.hasCondition();
	su.addEqualCondition("uid", uid);
	su.setConditionJoin(true);
	su.addEqualCondition("mail_id", mailId);
	if (!_client.query(query)) {
	    CLOG(WARNING) << "Set read mail for user [" << uid << "] failed.";
	    return false;
	}
    	return true;
    }

    bool SlotsDB::delMail(const std::string &uid, const std::string &mailId) {
	std::string query;
	SqlUpdate su(query);
	su.setTable("mail_info");
	su.updateValue("b_delete", "1");
	su.hasCondition();
	su.addEqualCondition("uid", uid);
	su.setConditionJoin(true);
	su.addEqualCondition("mail_id", mailId);
	if (!_client.query(query)) {
	    CLOG(WARNING) << "Del mail for user [" << uid << "] failed.";
	    return false;
	}
    	return true;
    }
    
    bool SlotsDB::getMailInfo(const MysqlRows &mails, UserMails &out){
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
	    tmp->mailInfo.attachment = (*itr)[9];
	    tmp->mailInfo.createTime = (*itr)[10];
	    tmp->mailInfo.keepDays = (*itr)[11];

	    tmp->bRead = (*itr)[2] == "0" ? false:true;
	    tmp->bGet = (*itr)[3] == "0" ? false:true;
	    tmp->bDel = (*itr)[4] == "0" ? false:true;
	    out.push_back(tmp);
	}
	return true;
    }

    bool SlotsDB::getFriendsList(
	const std::string &uid, uint32_t page,uint32_t pageSize, FriendsList &list)
    {
	std::string sQuery = "select * from user_info as A inner join user_resource as B on A.uid = B.uid and (A.uid in (select uid2 as uid from friends where uid1 = ";
	sQuery += uid;
	sQuery += ") or A.uid in (select uid1 as uid from friends where uid2  =";
	sQuery += uid;
	sQuery += ")) ";
	sQuery += "limit ";
	sQuery += cgserver::StringUtil::toString(page);
	sQuery += ",";
	sQuery += cgserver::StringUtil::toString(pageSize);

	MysqlRows res;
	if (!_client.queryWithResult(sQuery, res)) {
	    return false;
	}
	collectSlotsUsers(res, list);
	return true;
    }

    bool SlotsDB::getInviteHistory(const std::string &uid, FHistory &out) {
	std::string sQuery;
	SqlSelect ss(sQuery);
	ss.addTable("f_history");
	ss.addEqualCondition("uid", uid);
	MysqlRow res;
	if (!_client.queryWithResult(sQuery, res)) {
	    return false;
	}
	if (res.size() < 4) {
	    return false;
	}
	out.uid = res[0];
	cgserver::StringUtil::StrToInt32(res[1].c_str(), out.inviteCount);
	cgserver::StringUtil::StrToInt64(res[2].c_str(), out.totalReward);
	cgserver::StringUtil::StrToInt64(res[3].c_str(), out.rewardRemain);
	return true;
    }

    bool SlotsDB::updateFHistory(const std::string &uid, const std::string &key, const std::string &value) {
	std::string uQuery;
	SqlUpdate su(uQuery);
	su.setTable("f_history");
	su.updateValue(key, value);
	su.addEqualCondition("uid", uid);
	return _client.query(uQuery);
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

    bool SlotsDB::collectSlotsUser(const cgserver::MysqlRow &row, SlotsUser &su) const{
	if (row.size() < 12) {
	    return false;
	}
	UserInfo &ui = su.uInfo;
	UserResource &ur = su.uRes;
	//user_info:uid, mid, fname, lname, avatar, male, country
	ui.uid = row[0];
	ui.mid = row[1];
	ui.fname = row[2];
	//ui.lname = row[3];
	ui.avatar = row[4];
	ui.male = row[5];
	ui.country = row[6];
	// user_resource:uid, level, exp, fortune, vip_level
	// we should makesure things right.
	ur.uid = row[7];	
	cgserver::StringUtil::StrToUInt32(row[8].c_str(), ur.level);
	cgserver::StringUtil::StrToUInt64(row[9].c_str(), ur.exp);
	cgserver::StringUtil::StrToInt64(row[10].c_str(), ur.fortune);
	cgserver::StringUtil::StrToUInt32(row[11].c_str(), ur.vipLevel);
	return true;
    }

    bool SlotsDB::removeFriend(const std::string &uidStr, const std::string &tidStr){
	uint64_t uid;
	if (!StringUtil::StrToUInt64(uidStr.c_str(), uid))
	    return false;
	uint64_t tid;
	if (!StringUtil::StrToUInt64(tidStr.c_str(), tid))
	    return false;
	std::string query;
	cgserver::SqlDelete sd(query);
	sd.addTable("friends");
	if (tid > uid){
	    sd.addEqualCondition("uid1", uidStr);
	    sd.setConditionJoin(true);
	    sd.addEqualCondition("uid2", tidStr);	    
	}else if (tid < uid) {
	    sd.addEqualCondition("uid1", tidStr);
	    sd.setConditionJoin(true);
	    sd.addEqualCondition("uid2", uidStr);	    
	}else {
	    return false;
	}
	return _client.query(query);
    }

    bool SlotsDB::makeFriend(const std::string &uidStr, const std::string &tidStr) {
	uint64_t uid;
	if (!StringUtil::StrToUInt64(uidStr.c_str(), uid))
	    return false;
	uint64_t tid;
	if (!StringUtil::StrToUInt64(tidStr.c_str(), tid))
	    return false;
	std::string values = "";
	// bigger id in uid2
	if (tid > uid){
	    values += uidStr;
	    values += ",";
	    values += tidStr;
	}else if (tid < uid) {
	    values += tidStr;
	    values += ",";
	    values += uidStr;
	}else{
	    return false;
	}
	return _client.addRow("friends", "uid1, uid2", values, false);
    }

#undef APPEND_VALUE
#undef SET_VALUE    
}
