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
        static const std::string strProcedureOut = "@x";
        static const std::string strFailedUid = "0";

        std::string query = cgserver::StrCall;
        query += UserInfoStr::sCreateUserProcedure;
        query += cgserver::StrLeftBracket;
        quoteValue(_mid, true, query);
        query += cgserver::StrComma;
        query += strProcedureOut;
        query += cgserver::StrRightBracket;

        cgserver::MysqlRow result;

        if (!queryWithResult(conn, query, result)) {
            CLOG(WARNING) << "Create user " << _mid << " failed.";
            return false;
        }

        if (result.size() == 0) {
            CLOG(WARNING)<< "Cannot get user " << _mid << "'s uid.";
            return false;
        }

        _uid = result[0];
        if (_uid == strFailedUid) {
            CLOG(WARNING)<< "Create get user " << _mid << " falied, rollback.";
            return false;
        }

        endTransaction(conn, true);
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
