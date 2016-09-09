#ifndef MYSQLSIMPLEDELETE_H
#define MYSQLSIMPLEDELETE_H

#include "mysqloperationbase.h"

namespace cgserver{
    class MysqlSimpleDelete:public MysqlOperationBase{
    public:
        MysqlSimpleDelete();
        virtual ~MysqlSimpleDelete();

	virtual bool doOperation(MYSQL *conn);
    };
}
#endif
