#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/processors/loginprocessor.h>
#include <slots/slotsconfig.h>
#include <util/config.h>
#include <mysql/mysqlconnpool.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string cfg_file = "/home/licheng/workspace/CgServerPlus/server.cfg";

class TestLoginProcessor: public CxxTest::TestSuite 
{
public:
TestLoginProcessor():_inited(false){}

    virtual void setUp(){
	if (!_inited){
            ast_true(Config::getInstance().initConfig(cfg_file));
	    ast_true(MysqlConnPool::getInstance().init());
            ast_true(SlotsConfig::getInstance().init());
            auto &x = SlotsConfig::getInstance().loginCfg;
            for (auto &item: x.levelBonus) {
                    CLOG(INFO) << item.first << "," << item.second;
            }
	    _inited = true;
        }
    }

    virtual void tearDown() {
    }

    void test_process_login_2days( void )
    {
        //code
        GameContext gc;
        SlotsUserPtr su(new SlotsUser);
        su->uInfo.uid = "id1";
        su->gDetail.lastLogin = CTimeUtil::getCurrentTimeInSeconds() - 24 * 3600;
        su->gDetail.consitiveLogin = 1;
        su->uRes.vipLevel = 1;
        gc.user = su;
        LoginProcessor lp;
        lp.process(gc);
        ast_eq(2, su->gDetail.consitiveLogin);
        ast_eq(3000, gc.user->loginReward.daysReward);
        ast_eq(300, gc.user->loginReward.specialReward);
    }

    void test_process_login_1day(void)
    {
        GameContext gc;
        SlotsUserPtr su(new SlotsUser);
        su->uInfo.uid = "id1";
        su->gDetail.lastLogin = CTimeUtil::getCurrentTimeInSeconds() - 48 * 3600;
        su->gDetail.consitiveLogin = 5;
        su->uRes.vipLevel = 1;
        gc.user = su;
        LoginProcessor lp;
        lp.process(gc);
        ast_eq(1, su->gDetail.consitiveLogin);
        ast_eq(2000, gc.user->loginReward.daysReward);
        ast_eq(200, gc.user->loginReward.specialReward);
        CLOG(INFO) << "Runner:" << gc.user->loginReward.runnerReward;
    }

    void test_process_already_login(void)
    {
        GameContext gc;
        SlotsUserPtr su(new SlotsUser);
        su->uInfo.uid = "id1";
        su->uRes.vipLevel = 1;
        su->gDetail.lastLogin = CTimeUtil::getCurrentTimeInSeconds();
        su->gDetail.consitiveLogin = 5;
        gc.user = su;
        gc.user->loginReward.daysReward = 99;

        LoginProcessor lp;
        lp.process(gc);
        ast_eq(5, su->gDetail.consitiveLogin);
        ast_eq(99, gc.user->loginReward.daysReward);
    }

    void test_process_vip_0(void)
    {
        GameContext gc;
        SlotsUserPtr su(new SlotsUser);
        su->uInfo.uid = "id1";
        su->uRes.vipLevel = 0;
        su->gDetail.lastLogin = CTimeUtil::getCurrentTimeInSeconds() - 48 * 3600;
        gc.user = su;
        LoginProcessor lp;
        lp.process(gc);
        ast_eq(1, su->gDetail.consitiveLogin);
        ast_eq(2000, gc.user->loginReward.daysReward);
        ast_eq(0, gc.user->loginReward.specialReward);
    }
private:
    bool _inited;
};
