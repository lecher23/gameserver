#include "adduser2mysql.h"
namespace slots{
    AddUser2Mysql::AddUser2Mysql():_type(SMOT_INIT_USER){
    }
    
    AddUser2Mysql::~AddUser2Mysql(){
    }

    bool AddUser2Mysql::doOperation(MYSQL *conn){
	switch(_type){
	case SMOT_INIT_USER:
	    return initSlotsUser(conn);
	case SMOT_DUMP_SLOTS_USER:
	    return dumpSlotsUser(conn);
	}
    }

    void AddUser2Mysql::setSlotsUser(SlotsUserPtr input) {
	_slotsUser = input;
    }

    bool AddUser2Mysql::dumpSlotsUser(MYSQL *conn) {
	return false;
    }

    bool AddUser2Mysql::initSlotsUser(MYSQL *conn) {
	if (!startTransaction(conn)) {
	    CLOG(ERROR) << "Start transaction failed.";	    
	    return false;
	}

	std::string iQuery = "INSERT INTO user_info (mid) VALUES (\"";
	iQuery += _mid;
	iQuery += "\")";

	std::string sQuery = "SELECT uid FROM user_info WHERE mid=\"";
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
	    if (!addRow(conn, "history", "uid", _uid)) {
		CLOG(WARNING) << "Add new user ["<< _mid << "] failed.";
		break;
	    }
	    if (!addRow(conn, "g_history", "uid", _uid)) {
		CLOG(WARNING) << "Add new user ["<< _mid << "] failed.";
		break;
	    }
	    ret = true;
	}while(false);
	endTransaction(conn, ret);
	return true;
    }

    void AddUser2Mysql::setType(SlotsMysqlOpType type) {
	_type = type;
    }

    void AddUser2Mysql::setMid(const std::string &mid) {
	_mid = mid;
    }

    std::string AddUser2Mysql::getUid() {
	return _uid;
    }
}
