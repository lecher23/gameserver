#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysqlsimpleinsert.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;

const std::string lua_file = "/home/licheng/workspace/lua/main.lua";

class TestMysqlSimpleInsert : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_normal( void )
    {
            MysqlSimpleInsert msi;
            msi.setTable("a");
            msi.setField("f1");
            msi.addField("f2");
            msi.setValue("v1");
            msi.addValue("v2");
            ast_eq("INSERT INTO a ( f1,f2 ) VALUES ( \"v1\",\"v2\" ) ", msi.getQuery());
    }

    void test_single( void )
    {
            MysqlSimpleInsert msi;
            msi.setTable("a");
            msi.setField("f1");
            msi.setValue("v1");
            ast_eq("INSERT INTO a ( f1 ) VALUES ( \"v1\" ) ", msi.getQuery());
    }

};
