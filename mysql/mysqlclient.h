#ifndef MYSQLCLIENT_H
#define MYSQLCLIENT_H
#include <string>
#include <map>
#include <vector>
#include "mysql.h"
#include "../util/threadlock.h"

namespace cgserver{
    class MysqlClient{
    public:
	// Notice: we use key-value to indicate field-field_value in mysql.
	typedef std::map<std::string, std::string> RowValues;
	typedef std::vector<std::string> MysqlRow;
	typedef std::vector<MysqlRow> MysqlRows;
	typedef std::string MysqlStr;
	
	static const std::string S_ItemSep;
	static const std::string S_LeftBracket;
	static const std::string S_RightBracket;
	static const std::string S_Values;
	static const std::string S_DQuote;

	static MysqlClient &getInstance();	
        ~MysqlClient();

	bool initClient(const std::string &uri, const std::string &uname,
			const std::string &pwd,const std::string &db, int port = 3306);
	bool addRow(const std::string &tableName, const RowValues &rv);
	bool query(const std::string &q);
	//void log(const std::string &lg);

	bool queryWithResult(const std::string &q, MysqlRow &out);
	bool insertWithReturn(MysqlStr &insertQuery, MysqlStr &selectQuery, MysqlRow &out);
	
	uint64_t affectRows();	
	
    private:
        MysqlClient(){}
	MysqlClient(const MysqlClient&);
	MysqlClient & operator = (const MysqlClient &);
	
	bool exeQuery(const std::string &q);
	void appendValue(const std::string &sVal, std::string sDest);
	bool getSingleResult(MysqlRow &out);
	
	MYSQL _client;
	ThreadMutex _lock; //we should use "ke chong ru suo"
    };
}
#endif
