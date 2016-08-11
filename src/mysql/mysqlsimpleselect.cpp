#include "mysqlsimpleselect.h"
namespace cgserver{
    MysqlSimpleSelect::MysqlSimpleSelect(){
	_query = "SELECT ";
    }
    
    MysqlSimpleSelect::~MysqlSimpleSelect(){
    }

    bool MysqlSimpleSelect::doOperation(MYSQL *conn) {
	result.clear();
	return queryWithResult(conn, _query, result);
    }

    void MysqlSimpleSelect::setField(const MysqlStr &field) {
	_query += field;
    }

    void MysqlSimpleSelect::addField(const MysqlStr &field) {
	_query += ",";
	_query += field;
    }

    void MysqlSimpleSelect::setTable(const MysqlStr &table) {
	_query += " FROM ";
	_query += table;
    }    

    void MysqlSimpleSelect::addTable(const MysqlStr &table) {
	_query += ",";
	_query += table;
    }    
    
}
