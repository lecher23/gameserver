#ifndef SQLDELETE_H
#define SQLDELETE_H

#include <string>
#include "sqlstatement.h"

namespace cgserver{
    class SqlDelete: public SqlStatement{
    public:
        SqlDelete(std::string &str);
        virtual ~SqlDelete();

	void addTable(const std::string &tb);
    };
}
#endif
