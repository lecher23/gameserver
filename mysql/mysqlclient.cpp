#include "mysqlclient.h"
#include "errmsg.h"

namespace cgserver{

    const std::string MysqlClient::S_ItemSep = ",";
    const std::string MysqlClient::S_LeftBracket = "(";
    const std::string MysqlClient::S_RightBracket= ")";
    const std::string MysqlClient::S_Values = " values ";
    const std::string MysqlClient::S_DQuote = "\"";
    
    MysqlClient::~MysqlClient(){
	mysql_close(&_client);
    }

    MysqlClient &MysqlClient::getInstance(){
	static MysqlClient instance;
	return instance;
    }

    bool MysqlClient::initClient(const std::string &uri, const std::string &uname,
				 const std::string &pwd, const std::string &db, int port)
    {
	if (mysql_init(&_client) == NULL) {
	    return false;
	}
	//reconnect to server after lose connection.
	char value = 1;
	mysql_options(&_client, MYSQL_OPT_RECONNECT, &value);
	return (mysql_real_connect(&_client, uri.c_str(),uname.c_str(), pwd.c_str(),
				   db.c_str(), port, NULL, 0) != NULL);
    }

    void MysqlClient::log(const std::string &lg){
	std::string q = "";
	q.assign("insert into server_log(msg) values (\"");
	for (size_t i = 0; i < lg.size(); ++i) {
	    if (lg[i] == '"') {
		q.append(1, '\\');
		q.append(1, lg[i]);
	    }
	    q.append(1, lg[i]);
	}
	q.append("\")");
	exeQuery(q);
    }

    bool MysqlClient::addRow(const std::string &tableName, const RowValues &rv) {
	std::string q;
	q.assign("insert into ");
	q.append(tableName);
	if (rv.size() == 0){
	    q.append("() values ()");
	}else {
	    std::string fields;
	    std::string values;
	    RowValues::const_iterator it = rv.begin();
	    fields.append(S_LeftBracket);
	    fields.append(it->first);
	    values.append(S_LeftBracket);
	    appendValue(it->second, values);
	    while(it != rv.end()) {
		fields.append(S_ItemSep);		
		fields.append(it->first);
		values.append(S_ItemSep);
		appendValue(it->second, values);		
		++it;
	    }
	    fields.append(S_RightBracket);
	    values.append(S_RightBracket);
	    q.append(fields);
	    q.append(S_Values);
	    q.append(values);
	}
	return exeQuery(q);
    }

    void MysqlClient::appendValue(const std::string &sVal, std::string sDest) {
	sDest.append(S_DQuote);
	sDest.append(sVal);
	sDest.append(S_DQuote);	
    }

    uint64_t MysqlClient::affectRows(){
	return mysql_affected_rows(&_client);
    }

    bool MysqlClient::exeQuery(const std::string &q) {
	std::cout << "run query:"<< q << std::endl;
	_lock.lock();
	int ret = mysql_query(&_client, q.c_str());
	_lock.unlock();
	if (ret != 0) {
	    switch(ret) {
	    case CR_COMMANDS_OUT_OF_SYNC:
		std::cout << "ERROR:CR_COMMANDS_OUT_OF_SYNC" << std::endl;
		break;
	    case CR_SERVER_GONE_ERROR:
		std::cout << "ERROR:CR_SERVER_GONE_ERROR" << std::endl;
		break;
	    case CR_SERVER_LOST:
		std::cout << "ERROR:CR_SERVER_LOST" << std::endl;
		break;
	    default:
		std::cout << "Unknow error." << std::endl;
	    }
	    return false;
	}
	return true;
    }

    bool MysqlClient::query(const std::string &q) {
	int ret = mysql_query(&_client, q.c_str());
	if (ret != 0) {
	    std::cout << "Run query [" << q << "] failed. Error code:" << ret << std::endl;
	    return false;
	}
	MYSQL_RES *result = mysql_store_result(&_client);
	if (result) {
	    uint64_t numRes = mysql_num_rows(result);
	    std::cout << "Result number:"<< numRes << std::endl;
	    mysql_free_result(result);
	}
    }
}
