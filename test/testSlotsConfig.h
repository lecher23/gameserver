#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysqlconnpool.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;

const std::string config_file = "";

class TestSlotsConfig : public CxxTest::TestSuite
{
public:
TestSlotsConfig():_inited(false){}
    virtual void setUp(){
            if (!_inited){
                    AST_TRUE(MysqlConnPool::getInstance().init());
                    _inited = true;
            }
    }

    virtual void tearDown() {
    }

    void test_simple( void )
    {
        //code
    }

private:
    bool _inited;
};
