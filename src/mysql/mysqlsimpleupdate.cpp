#include "mysqlsimpleupdate.h"

BEGIN_NAMESPACE(cgserver)
MysqlSimpleUpdate::MysqlSimpleUpdate(){
    _query = StrUpdate;
}

MysqlSimpleUpdate::~MysqlSimpleUpdate(){
}

bool MysqlSimpleUpdate::doOperation(MYSQL *conn) {
    bool ret = exeQuery(conn, _query);
    endTransaction(conn, ret);
    return ret;
}

void MysqlSimpleUpdate::setUpdateValue(
    const std::string &field, const std::string &value, bool quoteVal)
{
    _query += StrSet;
    MAKE_COND_SENTENCE(field, value, quoteVal);
}

void MysqlSimpleUpdate::addUpdateValue(
    const std::string &field, const std::string &value, bool quoteVal)
{
    _query += StrComma;
    MAKE_COND_SENTENCE(field, value, quoteVal);
}
END_NAMESPACE
