#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "mysql/mysqlclient.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;

class MysqlClientTest : public CxxTest::TestSuite 
{
public:
 MysqlClientTest():_inited(false){}

    virtual void setUp(){

	if (!_inited){
	    AST_TRUE(MysqlClient::getInstance().initClient("127.0.0.1", "root", "111222", "SLOTS"));
	    _inited = true;
	}
	MysqlClient::getInstance().exeQuery("delete from user_info where mid=\"mysqlclienttest\"");	
    }

    virtual void tearDown() {
    }

    void test_transaction_rollback( void )
    {
	MysqlClient &client = MysqlClient::getInstance();
	AST_TRUE(client.startTransaction());
	AST_TRUE(client.exeQuery("insert into user_info (mid) values (\"mysqlclienttest\")"));
	AST_TRUE(client.endTransaction(false));
	MysqlRows out;
	AST_TRUE(client.queryWithResult("select * from user_info where mid=\"mysqlclienttest\"", out));
	AST_EQ(0, out.size());
    }

    void test_transaction_commit( void )
    {
	MysqlClient &client = MysqlClient::getInstance();
	AST_TRUE(client.startTransaction());
	AST_TRUE(client.exeQuery("insert into user_info (mid) values (\"mysqlclienttest\")"));
	AST_TRUE(client.endTransaction(true));
	MysqlRows out;
	AST_TRUE(client.queryWithResult("select * from user_info where mid=\"mysqlclienttest\"", out));
	AST_EQ(1, out.size());
    }

    void test_simple_insert_without_transaction(void)
    {
	MysqlClient &client = MysqlClient::getInstance();
	AST_TRUE(client.exeQuery("insert into user_info (mid) values (\"mysqlclienttest\")"));
	MysqlRows out;
	AST_TRUE(client.queryWithResult("select * from user_info where mid=\"mysqlclienttest\"", out));
	AST_EQ(1, out.size());
    }
    
 private:
    bool _inited;
};
