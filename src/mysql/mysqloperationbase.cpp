#include "mysqloperationbase.h"
#include "errmsg.h"

namespace cgserver{
    MysqlOperationBase::MysqlOperationBase(){
    }

    MysqlOperationBase::~MysqlOperationBase(){
    }

    bool MysqlOperationBase::exeQuery(MYSQL *conn, const MysqlStr &query) {
	CLOG(INFO) << "run query:"<< query ;
	int ret = mysql_query(conn, query.c_str());
	if (ret != 0) {
	    switch(ret) {
	    case CR_COMMANDS_OUT_OF_SYNC:
		CLOG(ERROR) << "ERROR:CR_COMMANDS_OUT_OF_SYNC" ;
		break;
	    case CR_SERVER_GONE_ERROR:
		CLOG(ERROR) << "ERROR:CR_SERVER_GONE_ERROR" ;
		break;
	    case CR_SERVER_LOST:
		CLOG(ERROR) << "ERROR:CR_SERVER_LOST" ;
		break;
	    default:
		CLOG(ERROR) << "Unknow error." ;
	    }
	    return false;
	}
	return true;
    }

    bool MysqlOperationBase::startTransaction(MYSQL * conn) {
	return exeQuery(conn, "START TRANSACTION");	
    }
    
    bool MysqlOperationBase::endTransaction(MYSQL *conn, bool commit){
	bool ret;
	if (commit) {
	    ret = exeQuery(conn, "COMMIT");
	} else {
	    ret = exeQuery(conn, "ROLLBACK");
	}
	return ret;
    }

    bool MysqlOperationBase::getResult(MYSQL *conn, MysqlRow &out) {
	MYSQL_RES *res = mysql_store_result(conn);
	// no result.
	if (res == NULL ) {
	    CLOG(ERROR) << "Store result from mysql failed.\n";
	    return false;
	}
	// char **
	MYSQL_ROW row = mysql_fetch_row(res);
	while(row != NULL) {
	    unsigned long *lens = mysql_fetch_lengths(res);
	    int fieldNum = mysql_num_fields(res);
	    for (size_t i = 0; i < fieldNum; ++i) {
		if (row[i] == NULL) {
		    out.push_back("");
		}else {
		    out.push_back(MysqlStr(row[i], lens[i]));
		}
	    }
	    // just one result
	    break;
	}
	// release result.
	mysql_free_result(res);
	return true;
    }
    
    bool MysqlOperationBase::getResult(MYSQL *conn, MysqlRows &out) {
	MYSQL_RES *res = mysql_store_result(conn);
	// no result.
	if (res == NULL ) {
	    CLOG(ERROR) << "Store result from mysql failed.\n";
	    return false;
	}
	// char **
	MYSQL_ROW row;
	unsigned long *lens;
	int fieldNum = mysql_num_fields(res);
	while((row = mysql_fetch_row(res))) {
	    lens = mysql_fetch_lengths(res);
	    MysqlRow singleRow;
	    for (size_t i = 0; i < fieldNum; ++i) {
		if (row[i] == NULL) {
		    singleRow.push_back("");
		}else {
		    singleRow.push_back(MysqlStr(row[i], lens[i]));
		}
	    }
	    out.push_back(singleRow);
	}
	// release result.
	mysql_free_result(res);
	return true;
    }    

    uint64_t MysqlOperationBase::affectRows(MYSQL *conn){
	return mysql_affected_rows(conn);
    }

    /*Get single result from mysql db*/
    bool MysqlOperationBase::queryWithResult(MYSQL *conn, const MysqlStr &q, MysqlRow &out) {
	out.clear();
	bool ret = false;
	do {
	    if (!exeQuery(conn, q)) {
		break;
	    }
	    if (!getResult(conn, out)) {
		break;
	    }
	    ret = true;
	}while(0);
	return ret;
    }

    /*Get lots result from mysql db*/
    bool MysqlOperationBase::queryWithResult(MYSQL *conn, const MysqlStr &q, MysqlRows &out) {
	out.clear();
	bool ret = false;
	do {
	    if (!exeQuery(conn, q)) {
		break;
	    }
	    if (!getResult(conn, out)) {
		break;
	    }
	    ret = true;
	}while(0);
	return ret;
    }

    bool MysqlOperationBase::insertWithReturn(
	MYSQL * conn, MysqlStr &insertQuery, MysqlStr &selectQuery, MysqlRow &out)
    {
	bool ret = false;
	do {
	    if (!exeQuery(conn, insertQuery)) {
		break;
	    }
	    if (!queryWithResult(conn, selectQuery, out)) {
		break;
	    }
	    ret = true;
	}while(0);
	return ret;
    }
    

    void MysqlOperationBase::setTable(const MysqlStr &table){
	_query += table;
    }

    void MysqlOperationBase::addTable(const MysqlStr &table) {
	_query += ",";
	_query += table;
    }
    
    void MysqlOperationBase::setCondition(
	const MysqlStr &left, const MysqlStr &right, bool quoteRight)
    {
	_query += " WHERE ";
	MAKE_COND_SENTENCE(left, right, quoteRight);
    }

    void MysqlOperationBase::setCondition(const MysqlStr &cond, bool quoteCond) {
	_query += " WHERE ";
	_query += quoteCond ? "(" : "";
	_query += cond;
	_query += quoteCond ? ")" : "";
    }

    void MysqlOperationBase::addCondition(
	const MysqlStr &left, const MysqlStr &right,
	bool conjAnd, bool quoteRight)
    {
	_query += conjAnd ? "AND" : "OR";
	MAKE_COND_SENTENCE(left, right, quoteRight);	
    }

    void MysqlOperationBase::addCondition(
	const MysqlStr &cond, bool conjAnd, bool quoteCond)
    {
	_query += conjAnd ? "AND" : "OR";
	_query += quoteCond ? "(" : "";
	_query += cond;
	_query += quoteCond ? ")" : "";
    }

    void MysqlOperationBase::setSortField(const MysqlStr &of, bool asc) {
	_query += " order by ";
	_query += of;
	_query += (asc ? " asc " : " desc ");
    }
    
    void MysqlOperationBase::setLimit(const MysqlStr &offset, const MysqlStr &size) {
	_query += " limit ";
	_query += offset;
	_query += ",";
	_query += size;	
    }

    void MysqlOperationBase::setLimit(uint32_t offset, uint32_t size) {
	_query += " limit ";
	_query += StringUtil::toString(offset);
	_query += ",";
	_query += StringUtil::toString(size);
    }
    
    void MysqlOperationBase::quoteValue(const MysqlStr &val, bool quote) {
	if (quote) {
	    _query += "\"";
	    _query += val;
	    _query += "\"";	    
	}else {
	    _query += val;
	}
    }
}
