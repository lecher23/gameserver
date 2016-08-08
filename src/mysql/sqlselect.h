#ifndef SQLSELECT_H
#define SQLSELECT_H

#include <string>
#include "sqlstatement.h"

namespace cgserver{
    class SqlSelect: public SqlStatement{
    public:
        SqlSelect(std::string &str);
        virtual ~SqlSelect();

	void addField(const std::string &fd);
	void addTable(const std::string &tb);
    };
}
#endif
