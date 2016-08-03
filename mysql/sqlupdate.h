#ifndef SQLUPDATE_H
#define SQLUPDATE_H

#include "sqlstatement.h"

namespace cgserver{
    class SqlUpdate:public SqlStatement{
    public:
        SqlUpdate(std::string &str);
        virtual ~SqlUpdate();
	// update table set x=x where
	void setTable(const std::string &tb);
	void updateValue(const std::string &key, const std::string &value);
	
    private:
	bool _hasKV;
    };
}
#endif
