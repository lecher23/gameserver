#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "slots/slotsdb.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

class SlotsDBTest : public CxxTest::TestSuite 
{
public:
 SlotsDBTest():_inited(false){}

    virtual void setUp(){

	if (!_inited){
	    AST_TRUE(MysqlClient::getInstance().initClient("127.0.0.1", "root", "111222", "SLOTS"));
	    _inited = true;
	}
	MysqlClient::getInstance().exeQuery("delete from user_info where mid=\"slotsdbtest\"");	
    }

    virtual void tearDown() {
    }

    void test_addUser_success(void)
    {
	SlotsDB &db = SlotsDB::getInstance();	
	MysqlClient &client = MysqlClient::getInstance();
	std::string uid;
	AST_TRUE(db.addUser("slotsdbtest", uid));
	MysqlRows out;
	AST_TRUE(client.queryWithResult("select * from user_info where mid=\"slotsdbtest\"", out));
	AST_EQ(1, out.size());
    }
    
 private:
    bool _inited;
};
