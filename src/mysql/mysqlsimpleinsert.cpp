#include "mysqlsimpleinsert.h"
BEGIN_NAMESPACE(cgserver)
MysqlSimpleInsert::MysqlSimpleInsert(){
    _query = StrInsertInto;
}

MysqlSimpleInsert::~MysqlSimpleInsert(){
}

bool MysqlSimpleInsert::doOperation(MYSQL *conn) {
    return exeQuery(conn, _query);
}

void MysqlSimpleInsert::setField(const std::string &field) {
    _query += StrLeftBracket;
    _query += field;
}

void MysqlSimpleInsert::addField(const std::string &field) {
    _query += StrComma;
    _query += field;
}

void MysqlSimpleInsert::setValue(const std::string &value) {
    _query += StrValues;
    quoteValue(value, true);
    _query += StrRightBracket;
}

void MysqlSimpleInsert::addValue(const std::string &value) {
    // remove last char: ")"
    _query.resize(_query.size() - 1);
    _query += StrComma;
    quoteValue(value, true);
    _query += StrRightBracket;
}
END_NAMESPACE
