#include "slotsdb.h"

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

    bool SlotsDB::getUserInfo(
	const std::string &mid, UserInfo &ui, std::string &errMsg) const
    {
	CLOG(INFO) << "Get user info\n";
	// get logic
	std::string sQuery = "select * from user_info as A inner join user_resource as B on A.uid = B.uid and A.mid = ";
	APPEND_VALUE(s, ui.mid);
	MysqlClient::MysqlRow res;
	if (!_client.queryWithResult(sQuery, res)) {
	    CLOG(ERROR) << "Run query [" << sQuery << "] failed.\n";
	    errMsg = "Run query failed.Code 110001.";
	    return false;
	}
	if (res.size() == 0) {
	    CLOG(INFO) << "New user. Record.\n";
	    std::string uid;
	    if (!addUser(mid, uid, errMsg)) {
		CLOG(ERROR) << "Set raw user info failed.\n";
		return false;
	    }
	    if (!_client.queryWithResult(sQuery, res)) {
		CLOG(ERROR) << "Run query [" << sQuery << "] failed.\n";
		return false;
	    }
	    if (res.size() == 0) {
		return false;
	    }
	}
	//user_info:uid, mid, fname, lname, avatar, male, country
	ui.uid = res[0];
	ui.mid = res[1];
	ui.fname = res[2];
	ui.lname = res[3];
	ui.avatar = res[4];
	ui.male = res[5];
	ui.country = res[6];
	//user_resource:uid, level, exp, fortune, vip_level
	ui.level = res[8];
	ui.exp = res[9];
	ui.fortune = res[10];
	ui.vip_level = res[11];
	return true;
    }

    bool SlotsDB::addUser(
	const std::string &mid, std::string &uid, std::string &errMsg) const 
    {
	// we should use transaction to make sure data ACID.
	// first we should forbid autocommit by using mysql_autocommit(&sql, false);
	std::string insertQuery = "insert into user_info (mid) values(";
	APPEND_VALUE(insert, mid);
	insertQuery.append(1, ')');
	
	std::string selectQuery = "select uid from user_info where mid=";
	APPEND_VALUE(select, mid);

	MysqlClient::MysqlRow out;
	if (!_client.insertWithReturn(insertQuery, selectQuery, out) || out.empty()) {
	    CLOG(ERROR) << "Add new user failed.\n";
	    return false;
	}
	uid = out[0];
	insertQuery.clear();
	insertQuery.append("insert into user_resource (uid) values(");
	APPEND_VALUE(insert, uid);
	insertQuery.append(1, ')');
	if (!_client.query(insertQuery)) {
	    return false;
	}
	// set logic
	return true;
    }

    bool SlotsDB::updateUserInfo(const UserInfo &ui, std::string &errMsg) const {
	// we should use transaction to make sure data ACID.
	// first we should forbid autocommit by using mysql_autocommit(&sql, false);
	std::string updateQuery = "UPDATE user_info SET ";
	SET_VALUE(update, "fname", ui.fname, true);
	SET_VALUE(update, "lname", ui.lname, true);
	SET_VALUE(update, "avatar", ui.avatar, true);
	SET_VALUE(update, "male", ui.male, true);
	SET_VALUE(update, "country", ui.country, true);
	SET_VALUE(update, "uid", ui.uid, false);
	updateQuery.append(" where uid = ");
	APPEND_VALUE(update, ui.uid);
	
	if (!_client.query(updateQuery)) {
	    return false;
	}
	return true;
    }
#undef APPEND_VALUE
#undef SET_VALUE    
}
