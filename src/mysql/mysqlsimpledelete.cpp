#include "mysqlsimpledelete.h"
BEGIN_NAMESPACE(cgserver)
MysqlSimpleDelete::MysqlSimpleDelete(){
    _query = StrDelete;
}

MysqlSimpleDelete::~MysqlSimpleDelete(){
}

bool MysqlSimpleDelete::doOperation(MYSQL *conn) {
    return exeQuery(conn, _query);
}

END_NAMESPACE
