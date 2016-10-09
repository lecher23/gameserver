#include "adduser2mysql.h"
#include <mysql/mysqlsimpleselect.h>
#include <mysql/mysqlsimpleinsert.h>

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

        cgserver::MysqlSimpleInsert msi;
        msi.setTable(UserInfoStr::sTableName);
        msi.setField(UserInfoStr::sMid);
        msi.setValue(_mid);
        auto iQuery = msi.getQuery();

        cgserver::MysqlSimpleSelect mss;
        mss.setField(UserInfoStr::sUid);
        mss.setTable(UserInfoStr::sTableName);
        mss.setCondition(UserInfoStr::sMid, _mid, true);
        auto sQuery = mss.getQuery();

	bool ret = false;
	do {
	    if (!insertWithReturn(conn, iQuery, sQuery, result)
		|| result.empty() || result[0].empty()) {
		CLOG(ERROR) << "Add new user failed.";
		break;
	    }
	    _uid = result[0][0];
	    if (!addRow(conn, UserResourceStr::sTableName,
                        UserResourceStr::sUid, _uid))
            {
		CLOG(WARNING) << "Add new user ["<< _mid << "] failed.";
		break;
	    }
	    if (!addRow(conn, GameHistoryStr::sTableName, UserInfoStr::sUid, _uid)) {
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
