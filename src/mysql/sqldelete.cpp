#include "sqldelete.h"
namespace cgserver{
    SqlDelete::SqlDelete(std::string &str):SqlStatement(str){
    }

    SqlDelete::~SqlDelete(){
    }

    void SqlDelete::addTable(const std::string &tb) {
	_str += "delete from ";
	_str += tb;
    }
}
