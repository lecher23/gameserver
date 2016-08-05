#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

#include "util/luatool.h"

using namespace cgserver;
class MyTestSuite : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_exefunc( void )
    {
        //code
	LuaTool tool;
	AST_TRUE(tool.load("/home/licheng/workspace/lua/main.lua"));
	int64_t val = 9;
	tool.chooseFunc("main");
	tool.pushValue(val);
	tool.pushValue(val);
	tool.pushValue(val);
	tool.pushValue(val);
	tool.pushValue("");
	AST_TRUE(tool.exeFunc( 5, 2));
    }
};
