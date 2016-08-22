#ifndef MYSQLCONNPOOL_H
#define MYSQLCONNPOOL_H
#include <string>
#include <map>
#include <vector>
#include "mysqloperationbase.h"
#include "mysqlsimpleselect.h"
#include "mysqlsimpleinsert.h"
#include "mysqlsimpledelete.h"
#include "mysqlsimpleupdate.h"

namespace cgserver{
    class MysqlConnPool{
    public:
	static MysqlConnPool &getInstance();	
        ~MysqlConnPool();

	bool init();
	bool readyToUse();
	bool doMysqlOperation(MysqlOperationBase *op);
	
    private:
        MysqlConnPool():_inited(false){}
	MysqlConnPool(const MysqlConnPool&);
	MysqlConnPool & operator = (const MysqlConnPool &);
	
	MYSQL *getConnect();
	void returnConnect(MYSQL *sql);
	bool createConnect();
	
	std::string _mysqlHost;
	std::string _userName;
	std::string _userPassword;
	std::string _databaseName;
	int _connPoolSize;
	int _mysqlPort;
	bool _inited;
	
	MYSQL _client;
	std::vector<MYSQL *> busyConn;
	std::vector<MYSQL *> idleConn;

	std::recursive_mutex _lock;
    };
    DF_SHARED_PTR(MysqlConnPool);
}
#endif
