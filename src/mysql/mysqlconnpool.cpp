#include "mysqlconnpool.h"
#include "errmsg.h"
#include "sqlselect.h"
#include "../util/stringutil.h"
#include "../util/config.h"

namespace cgserver{

    MysqlConnPool::~MysqlConnPool(){
	// wait conn finish their job.
	while(!busyConn.empty()) {
	    sleep(1);
	}
	for (auto val: idleConn) {
	    mysql_close(val);
	}
    }

    MysqlConnPool &MysqlConnPool::getInstance(){
	static MysqlConnPool instance;
	return instance;
    }

    bool MysqlConnPool::init()
    {
	/*Init db*/
	_mysqlHost = Config::getInstance().getConfigValue("db", "addr");
	_userName = Config::getInstance().getConfigValue("db", "user_name");
	_userPassword = Config::getInstance().getConfigValue("db", "user_pwd");
	_databaseName = Config::getInstance().getConfigValue("db", "db_name");
	if (!Config::getInstance().getIntValue("db", "poll_size", _connPoolSize) ||
	    _connPoolSize < 1 || _connPoolSize > MAX_POOL_SIZE)
	{
	    _connPoolSize = MAX_POOL_SIZE;
	}

	if (!Config::getInstance().getIntValue("db", "port", _mysqlPort) || _mysqlPort < 1)
	{
	    _mysqlPort = 3306;
	}
	
	if (_mysqlHost.empty() || _userName.empty() || _databaseName.empty()) {
	    CLOG(ERROR) << "Mysql connection info not set under section [db]";
	    return false;
	}

	for (int i = 0; i < _connPoolSize; ++i) {
	    if (!createConnect()) {
		return false;
	    }
	}
	return true;
    }

    bool MysqlConnPool::doMysqlOperation(MysqlOperationBase *op){
	MYSQL *conn = this->getConnect();
	if (conn == NULL) {
	    return false;
	}
	bool ret = op->doOperation(conn);
	returnConnect(conn);
	return ret;
    }    
    
/***Private method***/
    bool MysqlConnPool::createConnect() {
	MYSQL *sql = new MYSQL;
	bool ret = false;
	do {
	    if (mysql_init(sql) == NULL) {
		break;
	    }
	    //reconnect to server after lose connection.
	    char value = 1;
	    mysql_options(sql, MYSQL_OPT_RECONNECT, &value);
	    if (mysql_real_connect(sql, _mysqlHost.c_str(),_userName.c_str(),
				   _userPassword.c_str(),_databaseName.c_str(),
				   _mysqlPort, NULL, 0) == NULL)
	    {
		break;
	    }
	    returnConnect(sql);
	    ret = true;
	}while (false);
	if (!ret) {
	    delete sql;
	}
	return ret;
    }

    MYSQL *MysqlConnPool::getConnect() {
	std::lock_guard<std::recursive_mutex> rMutexGard(_lock);
	if (idleConn.empty()) {
	    if (!createConnect()) {
		return NULL;
	    }
	    CLOG(INFO) << "Create new sql connect to pool.";
	}
	MYSQL *ret = idleConn.back();
	idleConn.pop_back();
	return ret;
    }

    void MysqlConnPool::returnConnect(MYSQL *in) {
	std::lock_guard<std::recursive_mutex> rMutexGard(_lock);	
	idleConn.push_back(in);
    }
}
