#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/config/freegameconfig.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testFreeGameConfig: public CxxTest::TestSuite 
{
public:
    testFreeGameConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
        FreeGameConfig cfg;
        ast_true(cfg.initFromJsonFile("config_files/FreeGameConfigTest.json"));
        ast_eq(3, cfg.end());
        const auto &item = cfg.getItem(1);
        ast_eq(4, item.first);
        ast_eq(10, item.second);
    }
private:
    bool _inited;
};
