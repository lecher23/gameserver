#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#include <slots/model/lineconfig.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testLineConfig: public CxxTest::TestSuite 
{
public:
    testLineConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
      LineConfig lc;
      std::vector<std::string> row = {"1","12,13,14,15,16,17"};
      ast_true(lc.deserialize(row));
      ast_eq(6, lc.line.size());
      ast_eq(12, lc.line[0]);
      ast_eq(13, lc.line[1]);
      ast_eq(14, lc.line[2]);
      ast_eq(15, lc.line[3]);
      ast_eq(16, lc.line[4]);
      ast_eq(17, lc.line[5]);
    }
private:
    bool _inited;
};
