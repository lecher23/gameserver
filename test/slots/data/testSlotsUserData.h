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
        auto &client = RedisClientFactory::getClient();
        ast_eq(RC_SUCCESS, client.Del("L23"));
    }

    void test_operate_daily_reward( void )
    {
        LoginReward reward;
        reward.runnerIdx = 12;
        reward.runnerReward = 1000;
        reward.vipExtra = 2000;
        reward.recved = false;
        reward.dayReward = 3000;
        SlotsUserData sud;
        sud.setDailyReward("23", reward);

        LoginReward result;
        ast_true(sud.getDailyReward("23", result));
        ast_eq(12, result.runnerIdx);
        ast_eq(1000, result.runnerReward);
        ast_eq(2000, result.vipExtra);
        ast_eq(3000, result.dayReward);
        ast_true(!result.recved);

        sud.updateDailyReward("23", true);
        ast_true(sud.getDailyReward("23", result));
        ast_eq(12, result.runnerIdx);
        ast_true(result.recved);
    }

    void test_operate_online_info(){
        OnlineInfo oInfo;
        oInfo.recved = false;
        oInfo.rewardID = 12;
        oInfo.rewardLevel = 3;
        oInfo.onlineTime = 100;

        SlotsUserData sud;
        ast_true(sud.setOnlineInfo("23", oInfo));

        ast_eq(99, sud.incrOnlineTime("23", 99));

        OnlineInfo oInfo1;
        ast_true(sud.getOnlineInfo("23", oInfo1));
        ast_eq(99, oInfo1.onlineTime);
        ast_eq(12, oInfo1.rewardID);
        ast_eq(3, oInfo1.rewardLevel);
        ast_true(!oInfo1.recved);

        sud.recvGift("23", true);
        ast_true(sud.getOnlineInfo("23", oInfo1));
        ast_true(oInfo1.recved);
    }
private:
    bool _inited;
};

