#include "sqlselect.h"
namespace cgserver{
    SqlSelect::SqlSelect(std::string &str):SqlStatement(str){
    }

    SqlSelect::~SqlSelect(){
    }

    void SqlSelect::addField(const std::string &fd) {
	if (!_fieldSetted) {
	    _str += "select ";
	}else{
	    _str += ",";
	}
	_str += fd;
	_fieldSetted = true;
    }
    
    void SqlSelect::addTable(const std::string &tb) {
	if (!_fieldSetted) {
	    _str += "select * ";
	    _fieldSetted = true;
	}
	if (!_tableSetted) {
	    _str += " from ";
	}else{
	    _str += ",";
	}
	_str += tb;
	_tableSetted = true;
    }
}
