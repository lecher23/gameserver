#ifndef SQLEXECUTOR_H
#define SQLEXECUTOR_H

#include "mysql/mysqloperationbase.h"

BEGIN_NAMESPACE(slots)

class SQLExecutor: cgserver::MysqlOperationBase{
public:
    SQLExecutor() {}
    virtual ~SQLExecutor() {}

    void setQuery(const std::string &query) {
        _query = query;
    }

    virtual bool doOperation(MYSQL *conn) {
        bool ret = exeQuery(conn, _query);
        endTransaction(conn, true);
        return ret;
    }

private:
    std::string _query;
};

END_NAMESPACE
#endif
