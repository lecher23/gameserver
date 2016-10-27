#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/config/slotlinesconfig.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testLinesConfig: public CxxTest::TestSuite 
{
public:
    testLinesConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
        SlotLinesConfig cfg;
        ast_true(cfg.initFromJsonFile("config_files/LinesConfigTest.json"));
        ast_eq(30, cfg.end());
        const auto line = cfg.getLine(4);
        ast_eq(5, line.size());
        ast_eq(10, line[0]);
        ast_eq(6, line[1]);
        ast_eq(2, line[2]);
        ast_eq(8, line[3]);
        ast_eq(14, line[4]);
        ast_eq(5, cfg.getPointCount());
    }
private:
    bool _inited;
};
