#include "mysqlsimpleupdate.h"
namespace cgserver{
    MysqlSimpleUpdate::MysqlSimpleUpdate(){
	_query = "UPDATE ";
    }
    
    MysqlSimpleUpdate::~MysqlSimpleUpdate(){
    }

    bool MysqlSimpleUpdate::doOperation(MYSQL *conn) {
	return exeQuery(conn, _query);
    }

    void MysqlSimpleUpdate::setUpdateValue(
	const std::string &field, const std::string &value, bool quoteVal)
    {
	_query += " SET ";
	MAKE_COND_SENTENCE(field, value, quoteVal);
    }

    void MysqlSimpleUpdate::addUpdateValue(
	const std::string &field, const std::string &value, bool quoteVal)
    {
	_query += ", ";
	MAKE_COND_SENTENCE(field, value, quoteVal);
    }
    
}
