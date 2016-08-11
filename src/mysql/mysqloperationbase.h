#ifndef MYSQLOPERATIONBASE_H
#define MYSQLOPERATIONBASE_H

#include "../util/stringutil.h"
#include "mysql.h"
#include "../util/common_define.h"
#include <map>
#include <vector>
#include <string>

namespace cgserver{

#define MAKE_COND_SENTENCE(left, right, quote)	\
    _query += left;				\
    _query += "=";				\
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
	
	MysqlStr _query;
    };
}
#endif
