#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/sql/slotsdb.h>
#include <util/config.h>

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string cfg_file = "/home/licheng/workspace/CgServerPlus/server.cfg";

class SlotsDBTest : public CxxTest::TestSuite 
{
public:
 SlotsDBTest():_inited(false){}

    virtual void setUp(){

	if (!_inited){
            AST_TRUE(Config::getInstance().initConfig(cfg_file));
	    AST_TRUE(MysqlConnPool::getInstance().init());
	    _inited = true;
	}
    }

    virtual void tearDown() {
    }

    void test_getUserInfoByMachineId(void)
    {
            std::string mid = "1234";
            SlotsUser su;
            AST_TRUE(SlotsDB::getInstance().getUserInfoByMachineId(mid, su));
    }

    // slots user data
    void test_getByMid(void){
            std::string mid = "123456000";
            SlotsUserData sud;
    }

 private:
    bool _inited;
};
