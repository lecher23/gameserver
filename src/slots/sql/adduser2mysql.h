#ifndef ADDUSER2MYSQL_H
#define ADDUSER2MYSQL_H

#include <mysql/mysqloperationbase.h>
#include <slots/data/basicdata.h>

namespace slots{
    class AddUser2Mysql: cgserver::MysqlOperationBase{
    public:
	enum SlotsMysqlOpType{
	    SMOT_INIT_USER,
	    SMOT_DUMP_SLOTS_USER,
	    SMOT_SELECT_SLOTS_USER
	};

        AddUser2Mysql();
        virtual ~AddUser2Mysql();

	virtual bool doOperation(MYSQL *conn);

	void setMid(const std::string &mid);
	void setType(SlotsMysqlOpType type);
	void setSlotsUser(SlotsUserPtr input);

	bool initSlotsUser(MYSQL *conn);
	bool dumpSlotsUser(MYSQL *conn);
	bool selectSlotsUser(MYSQL *conn);

	std::string getUid();
	SlotsUserPtr getSlotsUser();

    private:
	std::string _mid;
	std::string _uid;
	SlotsUserPtr _slotsUser;

	SlotsMysqlOpType _type;
    };
}
#endif
