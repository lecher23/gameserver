#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <socket/future.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testFuture: public CxxTest::TestSuite 
{
public:
    testFuture():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
    }
private:
    bool _inited;
};
