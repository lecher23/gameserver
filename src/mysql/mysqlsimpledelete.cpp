#include "mysqlsimpledelete.h"
BEGIN_NAMESPACE(cgserver)
MysqlSimpleDelete::MysqlSimpleDelete(){
    _query = StrDelete;
}

MysqlSimpleDelete::~MysqlSimpleDelete(){
}

bool MysqlSimpleDelete::doOperation(MYSQL *conn) {
    bool ret = exeQuery(conn, _query);
    endTransaction(conn, ret);
    return ret;
}

END_NAMESPACE
