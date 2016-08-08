#ifndef SQLSTATEMENT_H
#define SQLSTATEMENT_H

#include <string>
#include "../util/stringutil.h"

namespace cgserver{
    class SqlStatement{
    public:
        SqlStatement(std::string &str);
        virtual ~SqlStatement();
	
	void addField(const std::string &fd);
	void addTable(const std::string &tb);

	void hasCondition();
	void addEqualCondition(const std::string &left, const std::string &right);
	void setCondition(const std::string &cd);
	void setConditionJoin(bool isAnd);
	void setOrderField(const std::string &of, bool asc);
	void setLimit(const std::string &offset, const std::string &size);
	void setLimit(uint32_t offset, uint32_t size);
	void innerJoin(const std::string &left, const std::string &right,
		       const std::string &leftKey, const std::string &rightKey);
	
    protected:
	void strJoin(const std::string &left, const std::string &right, const std::string &seq);
	
	std::string &_str;
	bool _condSetted;
	bool _fieldSetted;
	bool _tableSetted;
    };
}
#endif
