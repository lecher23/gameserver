#ifndef MYSQLSIMPLEINSERT_H
#define MYSQLSIMPLEINSERT_H

#include "mysqloperationbase.h"

namespace cgserver{
    class MysqlSimpleInsert:public MysqlOperationBase{
    public:
        MysqlSimpleInsert();
        virtual ~MysqlSimpleInsert();

	virtual bool doOperation(MYSQL *conn);

	void setField(const std::string &field);
	void addField(const std::string &field);
	void setValue(const std::string &value);
	void addValue(const std::string &value);
    };
}
#endif
