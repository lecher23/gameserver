#ifndef MYSQLCLIENT_H
#define MYSQLCLIENT_H
#include <string>
#include <map>
#include "mysql.h"
#include "../util/threadlock.h"

namespace cgserver{
    class MysqlClient{
    public:
	// Notice: we use key-value to indicate field-field_value in mysql.
	typedef std::map<std::string, std::string> RowValues;
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
	bool exeQuery(const std::string &q);
	bool query(const std::string &q);
	void log(const std::string &lg);
	
	uint64_t affectRows();	
	
    private:
        MysqlClient(){}
	MysqlClient(const MysqlClient&);
	MysqlClient & operator = (const MysqlClient &);
	
	void appendValue(const std::string &sVal, std::string sDest);
	
	MYSQL _client;
	ThreadMutex _lock;
    };
}
#endif
