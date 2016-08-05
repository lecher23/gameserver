#ifndef SQLSTATEMENT_H
#define SQLSTATEMENT_H

#include <string>

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
	
    protected:
	std::string &_str;
	bool _condSetted;
	bool _fieldSetted;
	bool _tableSetted;
    };
}
#endif
