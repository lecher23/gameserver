#include "batchsql.h"

BEGIN_NAMESPACE(slots)

BatchSql::BatchSql(std::vector<std::string> &data): _data(data){
}

BatchSql::~BatchSql(){
}

bool BatchSql::doOperation(MYSQL *conn) {
    startTransaction(conn);
    for (auto &str: _data) {
	exeQuery(conn, str);
    }
    endTransaction(conn, true);
    return true;
}

END_NAMESPACE
