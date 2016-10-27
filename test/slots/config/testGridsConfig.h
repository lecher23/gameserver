#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/config/gridsconfig.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testGridsConfig: public CxxTest::TestSuite 
{
public:
    testGridsConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
        GridsConfig cfg;
        ast_true(cfg.initFromJsonFile("config_files/GridsConfigTest.json"));
        ast_eq(180, cfg.end());
        const auto &item = cfg.getGrid(1);
        ast_eq(0, item.col);
        ast_eq(1, item.row);
        ast_eq(1001, item.eleID);
        ast_eq(0, item.weight);
    }
private:
    bool _inited;
};
