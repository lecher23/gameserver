#include "sqlupdate.h"
namespace cgserver{
    SqlUpdate::SqlUpdate(std::string &str):SqlStatement(str), _hasKV(false){
    }

    SqlUpdate::~SqlUpdate(){
    }

    void SqlUpdate::setTable(const std::string &tb) {
	_str += "update ";
	_str += tb;
    }
    
    void SqlUpdate::updateValue(const std::string &key, const std::string &value) {
	if (!_hasKV) {
	    _str += " set ";
	} else {
	    _str += ",";
	}
	_str += key;
	_str += "=";
	_str += "\"";
	_str += value;
	_str += "\"";
    }
    
}
