#include "sqlstatement.h"
namespace cgserver{
    SqlStatement::SqlStatement(std::string &str):
	_str(str),_fieldSetted(false),_tableSetted(false)
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
	_str += " where " ;
    }

    // key="val" 
    void SqlStatement::addEqualCondition(const std::string &left, const std::string &right) {
	_str += "(";
	_str += left;
	_str += "=\"";
	_str += right;
	_str += "\")";
    }
    
    void SqlStatement::setCondition(const std::string &cd) {
	_str += " where ";
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
    
    void SqlStatement::setLimit(std::string &offset, std::string &size) {
	_str += " limit ";
	_str += offset;
	_str += ",";
	_str += size;	
    }
}
