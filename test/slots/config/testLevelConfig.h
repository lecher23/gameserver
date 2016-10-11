#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#include <slots/config/levelconfig.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testLevelConfig: public CxxTest::TestSuite 
{
public:
    testLevelConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_init_from_json_file( void )
    {
        LevelConfig lc;
        ast_true(lc.initFromJsonFile("config_files/Level.json"));
        ast_eq(200, lc.config.size());
        ast_eq(194, lc.config[194].level);
        ast_eq(201401803930000, lc.config[194].expNeed);
        ast_eq(10000000, lc.config[194].maxBet);
        ast_eq(49000, lc.config[194].fortuneReward);
    }
private:
    bool _inited;
};
