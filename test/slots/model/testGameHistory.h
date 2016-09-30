#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#include <slots/model/gamehistory.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testGameHistory: public CxxTest::TestSuite 
{
public:
    testGameHistory():_inited(false){}

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
