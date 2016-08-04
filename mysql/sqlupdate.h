#ifndef SQLUPDATE_H
#define SQLUPDATE_H

#include "sqlstatement.h"
#include <sstream>

namespace cgserver{
    class SqlUpdate:public SqlStatement{
    public:
        SqlUpdate(std::string &str);
        virtual ~SqlUpdate();
	// update table set x=x where
	void setTable(const std::string &tb);
	void updateValue(const std::string &key, const std::string &value);

	template <typename T>
	    void updateVal(const std::string &key, T value) {
	    std::ostringstream oss;
	    oss << value;
	    std::string out = oss.str();
	    updateValue(key, out);
	}
	
    private:
	bool _hasKV;
    };
}
#endif
