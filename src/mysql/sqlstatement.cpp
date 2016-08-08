#include "sqlstatement.h"

namespace cgserver{
    SqlStatement::SqlStatement(std::string &str):
	_str(str),_condSetted(false),_fieldSetted(false),_tableSetted(false)
    {
    }
    
    SqlStatement::~SqlStatement(){
    }

    void SqlStatement::addField(const std::string &fd) {
	_fieldSetted = true;
    }
    
    void SqlStatement::addTable(const std::string &tb) {
	_tableSetted = true;
    }

    void SqlStatement::hasCondition() {
	if (!_condSetted){
	    _str += " where ";
	    _condSetted = true;
	}
    }

    // key="val" 
    void SqlStatement::addEqualCondition(const std::string &left, const std::string &right) {
	hasCondition();
	_str += "(";
	_str += left;
	_str += "=\"";
	_str += right;
	_str += "\")";
    }
    
    void SqlStatement::setCondition(const std::string &cd) {
	hasCondition();
	_str += cd;
    }

    void SqlStatement::setConditionJoin(bool isAnd) {
	_str += (isAnd?" and ":" or ");
    }

    void SqlStatement::setOrderField(const std::string &of, bool asc) {
	_str += " order by ";
	_str += of;
	_str += (asc ? " asc " : " desc ");
    }
    
    void SqlStatement::setLimit(const std::string &offset, const std::string &size) {
	_str += " limit ";
	_str += offset;
	_str += ",";
	_str += size;	
    }

    void SqlStatement::setLimit(uint32_t offset, uint32_t size) {
	_str += " limit ";
	_str += StringUtil::toString(offset);
	_str += ",";
	_str += StringUtil::toString(size);
    }
    
    void SqlStatement::innerJoin(const std::string &left, const std::string &right,
				 const std::string &leftKey, const std::string &rightKey)
    {
	_str += " from ";
	_str += left;
	_str += " inner join ";
	_str += right;
	_str += " on ";
	strJoin(left, leftKey, ".");
	_str += "=";
	strJoin(right, rightKey, ".");
	_condSetted = true;
    }

    void SqlStatement::strJoin(
	const std::string &left, const std::string &right, const std::string &seq)
    {
	_str += left;
	_str += seq;
	_str += right;
    }    
}
