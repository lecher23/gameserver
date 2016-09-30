#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testResultFormatter: public CxxTest::TestSuite 
{
public:
    testResultFormatter():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_formatSimpleResult( void )
    {
      SBuf buf;
      ResultFormatter rf(buf);
      GameContext gc;
      gc.user = std::make_shared<SlotsUser>();

      rf.formatSimpleResult(false, "");
      
    }
private:
    bool _inited;
};
