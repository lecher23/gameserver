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
        LoginReward reward;
        reward.runnerIdx = 12;
        reward.runnerReward = 1000;
        reward.vipExtra = 2000;
        reward.recved = false;
        reward.dayReward = 3000;
        SlotsUserData sud;
        sud.setDailyReward("123", reward);

        LoginReward result;
        ast_true(sud.getDailyReward("123", result));
        ast_eq(12, result.runnerIdx);
        ast_eq(1000, result.runnerReward);
        ast_eq(2000, result.vipExtra);
        ast_eq(3000, result.dayReward);
        ast_true(!result.recved);

        sud.updateDailyReward("123", true);
        ast_true(sud.getDailyReward("123", result));
        ast_eq(12, result.runnerIdx);
        ast_true(result.recved);
    }
private:
    bool _inited;
};

