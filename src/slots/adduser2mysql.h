#ifndef ADDUSER2MYSQL_H
#define ADDUSER2MYSQL_H

#include "../mysql/mysqloperationbase.h"

namespace cgserver{
    class AddUser2Mysql: MysqlOperationBase{
    public:
        AddUser2Mysql();
        virtual ~AddUser2Mysql();

	virtual bool doOperation(MYSQL *conn);

	void setMid(const std::string &mid);
	std::string getUid();

    private:
	std::string _mid;
	std::string _uid;
    };
}
#endif
