#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/processors/gameprocessor.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;
const std::string lua_file = "/home/licheng/workspace/lua/main.lua";

class MyTestSuite : public CxxTest::TestSuite
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_simple( void )
    {
        //code
    }
};
