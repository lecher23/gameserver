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
    addUpdateValue(field, value, quoteVal);
}

void MysqlSimpleUpdate::addUpdateValue(
    const std::string &field, const std::string &value, bool quoteVal)
{
    if (valueCount > 0) {
        _query += StrComma;
    } else {
        _query += StrSet;
    }
    MAKE_COND_SENTENCE(field, value, quoteVal);
    ++valueCount;
}
END_NAMESPACE
