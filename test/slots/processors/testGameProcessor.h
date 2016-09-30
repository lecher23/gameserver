#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/processors/gameprocessor.h>
#include <slots/data/gamecontext.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"


#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;
const std::string lua_file = "/home/licheng/workspace/lua/main.lua";

class MyTestSuite : public CxxTest::TestSuite
{
public:
    virtual void setUp(){
        CommonTools::init_slots_config_tsconfig();
        CommonTools::init_slots_data_center_data();
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        GameContext gc;
        gc.user = std::make_shared<SlotsUser>();
        gc.user->uRes.fortune = 1000;

        gc.uid = 1024;
        gc.roomID = 33;
        gc.hallID = 22;

        gc.gameInfo.bet = 1000;
        gc.gameInfo.lineNumber = 5;

        GameProcessor gp;
        ast_true(gp.process(gc));

        auto &ts_cfg = SlotsConfig::getInstance().themeConfig.tsConfig;
        ast_eq(ts_cfg.getColumnNumber() * ts_cfg.getRowNumber(),
               gc.gameInfo.gridsData.size());
    }
};
