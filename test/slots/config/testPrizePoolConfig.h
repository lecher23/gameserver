#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/config/prizepoolconfig.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testPrizePoolConfig: public CxxTest::TestSuite 
{
public:
    testPrizePoolConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
        PrizePoolConfig cfg;
        ast_true(cfg.initFromJsonFile("config_files/PrizePoolConfigTest.json"));
        const auto &room = cfg.getRoomConfig();
        ast_eq(1, room.id);
        ast_eq(500, room.minBet);
        ast_eq(500000, room.base);
        ast_true(0.09< room.tax && room.tax < 0.11);

        const auto &hall = cfg.getHallConfig();
        ast_eq(2, hall.id);
        ast_eq(1000, hall.minBet);
        ast_eq(1000000, hall.base);
        ast_true(0.02< hall.tax && hall.tax < 0.04);
    }
private:
    bool _inited;
};
