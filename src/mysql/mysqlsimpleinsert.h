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
        void updateIfExist();// on duplicat key update
        void setFieldValue(const std::string &field, const std::string &value);
        void addFieldValue(const std::string &field, const std::string &value);
    protected:
        void setField(const MysqlStr &field, MysqlStr &query);
        void addField(const MysqlStr &field, MysqlStr &query);
        void setValue(const MysqlStr &value, MysqlStr &query);
        void addValue(const MysqlStr &value, MysqlStr &query);
    };
}
#endif
