#include "mysqlsimpleinsert.h"
namespace cgserver{
    MysqlSimpleInsert::MysqlSimpleInsert(){
	_query = "INSERT INTO ";
    }
    
    MysqlSimpleInsert::~MysqlSimpleInsert(){
    }

    bool MysqlSimpleInsert::doOperation(MYSQL *conn) {
	return exeQuery(conn, _query);
    }

    void MysqlSimpleInsert::setField(const std::string &field) {
	_query += "(";
	_query += field;
    }

    void MysqlSimpleInsert::addField(const std::string &field) {
	_query += ",";
	_query += field;
    }

    void MysqlSimpleInsert::setValue(const std::string &value) {
	_query += ") VALUES (";
	quoteValue(value, true);
	_query += ")";
    }

    void MysqlSimpleInsert::addValue(const std::string &value) {
	// remove last char: ")"
	_query.resize(_query.size() - 1);
	_query += ",";
	quoteValue(value, true);	
	_query += ")";	
    }

}
