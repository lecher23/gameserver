#include "adduser2mysql.h"
namespace cgserver{
    AddUser2Mysql::AddUser2Mysql(){
    }
    
    AddUser2Mysql::~AddUser2Mysql(){
    }

    bool AddUser2Mysql::doOperation(MYSQL *conn){
	if (!startTransaction(conn)) {
	    CLOG(ERROR) << "Start transaction failed.";	    
	    return false;
	}

	MysqlStr iQuery = "INSERT INTO user_info (mid) VALUES (\"";
	iQuery += _mid;
	iQuery += "\")";

	MysqlStr sQuery = "SELECT uid FROM user_info WHERE mid=\"";
	sQuery += _mid;
	sQuery += "\"";
	bool ret = false;
	do {
	    if (!insertWithReturn(conn, iQuery, sQuery, result)
		|| result.empty() || result[0].empty()) {
		CLOG(ERROR) << "Add new user failed.";
		break;
	    }
	
	    _uid = result[0][0];
	    if (!addRow(conn, "user_resource", "uid", _uid)) {
		CLOG(WARNING) << "Add new user ["<< _mid << "] failed.";
		break;
	    }
	    if (!addRow(conn, "f_history", "uid", _uid)) {
		CLOG(WARNING) << "Add new user ["<< _mid << "] failed.";
		break;
	    }
	    ret = true;
	}while(false);
	endTransaction(conn, ret);
	return true;
    }

    void AddUser2Mysql::setMid(const std::string &mid) {
	_mid = mid;
    }

    std::string AddUser2Mysql::getUid() {
	return _uid;
    }
}
