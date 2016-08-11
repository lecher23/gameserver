#ifndef MYSQLSIMPLEUPDATE_H
#define MYSQLSIMPLEUPDATE_H

#include "mysqloperationbase.h"

namespace cgserver{
    class MysqlSimpleUpdate:public MysqlOperationBase{
    public:
        MysqlSimpleUpdate();
        virtual ~MysqlSimpleUpdate();
	
	virtual bool doOperation(MYSQL *conn);

	void setUpdateValue(const std::string &field, const std::string &value, bool quoteValue = true);
	
	void addUpdateValue(const std::string &field, const std::string &value, bool quoteValue = true);
    };
}
#endif
