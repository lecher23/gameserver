#ifndef MYSQLSIMPLESELECT_H
#define MYSQLSIMPLESELECT_H

#include "mysqloperationbase.h"

namespace cgserver{
    class MysqlSimpleSelect:public MysqlOperationBase{
    public:
        MysqlSimpleSelect();
        virtual ~MysqlSimpleSelect();
	
	virtual bool doOperation(MYSQL *conn);

	void addField(const MysqlStr &field);
	void setField(const MysqlStr &field);
	void addTable(const MysqlStr &table);
	void setTable(const MysqlStr &table);

	MysqlRows result;
    };
}
#endif
