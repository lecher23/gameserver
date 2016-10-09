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
    _query.resize(_query.size() - StrRightBracket.size());
    _query += StrComma;
    quoteValue(value, true);
    _query += StrRightBracket;
}

void MysqlSimpleInsert::setField(const std::string &field, std::string &query) {
    query += StrLeftBracket;
    query += field;
}

void MysqlSimpleInsert::addField(const std::string &field, std::string &query) {
    query += StrComma;
    query += field;
}

void MysqlSimpleInsert::setValue(const std::string &value, std::string &query) {
    query += StrValues;
    quoteValue(value, true, query);
    query += StrRightBracket;
}

void MysqlSimpleInsert::addValue(const std::string &value, std::string &query) {
    // remove last char: ")"
    query.resize(query.size() - StrRightBracket.size());
    query += StrComma;
    quoteValue(value, true, query);
    query += StrRightBracket;
}

END_NAMESPACE
