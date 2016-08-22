#ifndef BATCHSQL_H
#define BATCHSQL_H

#include <mysql/mysqloperationbase.h>

BEGIN_NAMESPACE(slots)

class BatchSql: cgserver::MysqlOperationBase{
 public:
    explicit BatchSql(std::vector<std::string> &data);
    virtual ~BatchSql();

    virtual bool doOperation(MYSQL *conn);

 private:
    std::vector<std::string> &_data;
};

END_NAMESPACE
#endif
