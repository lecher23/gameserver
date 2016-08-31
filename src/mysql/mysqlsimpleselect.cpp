#include "mysqlsimpleselect.h"
BEGIN_NAMESPACE(cgserver)
MysqlSimpleSelect::MysqlSimpleSelect(){
    _query = StrSelect;
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
    _query += StrComma;
    _query += field;
}

void MysqlSimpleSelect::setTable(const MysqlStr &table) {
    _query += StrFrom;
    _query += table;
}

void MysqlSimpleSelect::addTable(const MysqlStr &table) {
    _query += StrComma;
    _query += table;
}
END_NAMESPACE
