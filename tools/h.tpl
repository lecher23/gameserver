#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y)
#define AST_TRUE(x) TS_ASSERT(x)

using namespace cgserver;
class MyTestSuite : public CxxTest::TestSuite 
{
public:
    void test_simple( void )
    {
        //code
    }
};