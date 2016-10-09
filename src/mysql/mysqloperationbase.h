#ifndef MYSQLOPERATIONBASE_H
#define MYSQLOPERATIONBASE_H

#include <mysql.h>
#include <map>
#include <vector>
#include <string>
#include <util/stringutil.h>
#include <util/common_define.h>

BEGIN_NAMESPACE(cgserver)
const std::string StrEqual = "=";
const std::string StrEmpty = "";
const std::string StrQuote = "\"";
const std::string StrComma = ",";
const std::string StrDot = ".";
const std::string StrStartTransaction = "START TRANSACTION";
const std::string StrCommit = "COMMIT";
const std::string StrRollback = "ROLLBACK";
const std::string StrInsertInto = "INSERT INTO ";
const std::string StrLeftBracket = " ( ";
const std::string StrRightBracket = " ) ";
const std::string StrWhereCond = " WHERE ";
const std::string StrAnd = " AND ";
const std::string StrOr = " OR ";
const std::string StrOrder = " ORDER BY ";
const std::string StrAsc = " ASC ";
const std::string StrDesc = " DESC ";
const std::string StrLimit = " LIMIT ";
const std::string StrFrom = " FROM ";
const std::string StrInnerJoin = " INNER JOIN ";
const std::string StrOn = " ON ";
const std::string StrValues = " ) VALUES ( ";
const std::string StrSelect = "SELECT ";
const std::string StrUpdate = "UPDATE ";
const std::string StrSet = " SET ";
const std::string StrDelete = "DELETE FROM ";

#define MAKE_COND_SENTENCE(left, right, quote)	\
    _query += left;				\
    _query += StrEqual;				\
    quoteValue(right, quote);

    typedef std::map<std::string, std::string> RowValues;
    typedef std::vector<std::string> MysqlRow;
    typedef std::vector<MysqlRow> MysqlRows;
    typedef std::string MysqlStr;

class MysqlOperationBase{
public:
        MysqlOperationBase();
        virtual ~MysqlOperationBase();

	virtual bool doOperation(MYSQL *conn) = 0;

	bool exeQuery(MYSQL *conn, const MysqlStr &query);
	/*startTransaction will try to get lock, but won't release lock*/
	bool startTransaction(MYSQL *conn);
	/*end transaction release lock, but won't get lock*/
	bool endTransaction(MYSQL *conn, bool commit);
	bool getResult(MYSQL *conn, MysqlRow &out);
	bool getResult(MYSQL *conn, MysqlRows &out);
	bool queryWithResult(MYSQL *conn, const MysqlStr &q, MysqlRow &out);
	bool queryWithResult(MYSQL *conn, const MysqlStr &q, MysqlRows &out);
	bool insertWithReturn(MYSQL * conn, MysqlStr &insertQuery,
			      MysqlStr &selectQuery, MysqlRows &out);
	bool addRow(MYSQL *conn, const MysqlStr &tableName, const MysqlStr &field,
		    const MysqlStr &value, bool quoteValue = true);
	uint64_t affectRows(MYSQL *conn);

        std::string getQuery();
	void setQuery(const MysqlStr &query);
	void setTable(const MysqlStr &table);
	void addTable(const MysqlStr &table);
	void setCondition(const MysqlStr &left, const MysqlStr &right, bool quoteVal);
	void setCondition(const MysqlStr &cond, bool quoteCond);
	void addCondition(const MysqlStr &left, const MysqlStr &right, bool conjAnd, bool quoteRight);
	void addCondition(const MysqlStr &cond, bool conjAnd, bool quoteCond);
	void setLimit(uint32_t offset, uint32_t size);
	void setLimit(const MysqlStr &offset, const MysqlStr &size);
	void setSortField(const MysqlStr &of, bool asc);
	void innerJoin(const MysqlStr &left, const MysqlStr &right,
		       const MysqlStr &leftKey, const MysqlStr &rightKey);

	MysqlRows result;
protected:
	void strJoin(const MysqlStr &left, const MysqlStr &right, const MysqlStr &seq);
	void quoteValue(const MysqlStr &val, bool quote);

        void setTable(const MysqlStr &table, MysqlStr &query);
        void quoteValue(const MysqlStr &val, bool quote, MysqlStr &query);

	MysqlStr _query;
};
END_NAMESPACE
#endif
