#include "mysqlsimpledelete.h"
namespace cgserver{
    MysqlSimpleDelete::MysqlSimpleDelete(){
	_query = "DELETE FROM ";
    }
    
    MysqlSimpleDelete::~MysqlSimpleDelete(){
    }

    bool MysqlSimpleDelete::doOperation(MYSQL *conn) {
	return exeQuery(conn, _query);
    }
}
