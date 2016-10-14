#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/data/slotsuserdata.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testSlotsUserData: public CxxTest::TestSuite 
{
public:
    testSlotsUserData():_inited(false){
        if (!_inited) {
            ast_true(RedisClientFactory::getClient().Initialize(
                         "127.0.0.1", 6379, 2, 3));
            _inited = true;
        }
    }

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        SlotsUserData sud;
        sud.setDailyReward("123", 12, false);

        int32_t runnerIdx;
        bool recved;
        ast_true(sud.getDailyReward("123", runnerIdx, recved));
        ast_eq(12, runnerIdx);
        ast_true(!recved);

        sud.updateDailyReward("123", true);
        ast_true(sud.getDailyReward("123", runnerIdx, recved));
        ast_eq(12, runnerIdx);
        ast_true(recved);
    }
private:
    bool _inited;
};
