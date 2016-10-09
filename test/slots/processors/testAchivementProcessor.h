#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/data/cjqueue.h>
#include <slots/data/slotsdatacenter.h>
#include "slots/processors/achievementprocessor.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string lua_file = "/home/licheng/workspace/lua/main.lua";

class TestAchievementProcessor : public CxxTest::TestSuite
{
public:

    virtual void setUp(){
        SlotsDataCenter::instance().cjQueue.reset(new CjQueue);
    }

    virtual void tearDown() {
    }

    CjSetting createCjSetting(int32_t id, int64_t target) {
        CjSetting y;
	y.id = id;
	y.target = target;
	y.reward = 1000;
	y.reward_type = 2;
	y.type = 0;
	return y;
    }

    void createGameContext(GameContext &ctx) {
	ctx.user = SlotsUserPtr(new SlotsUser);
	ctx.user->uInfo.uid = "9";
    }

    void test_level_up_get_cj( void )
    {
	auto &cfg = SlotsConfig::getInstance().cjConfig;
	auto &x = cfg[EGE_LEVEL_UP];
	x.clear();
	x.push_back(createCjSetting(110, 10));
	EventInfo e(EGE_LEVEL_UP, 10);
	AchievementProcessor p;
	GameContext cxt;
	createGameContext(cxt);
	cxt.events.push_back(e);
	p.process(cxt);
	AST_EQ(1, cxt.userCj.size());
	AST_EQ(false, cxt.userCj[0].isRecvReward);
	AST_EQ(9, cxt.userCj[0].uid);
	AST_EQ(110, cxt.userCj[0].aid);
    }

    void test_leve_up_not_get_cj(void)
    {
	auto &cfg = SlotsConfig::getInstance().cjConfig;
	auto &x = cfg[EGE_LEVEL_UP];
	x.clear();
	x.push_back(createCjSetting(110, 10));
	EventInfo e(EGE_LEVEL_UP, 10, 11);
	AchievementProcessor p;
	GameContext cxt;
	createGameContext(cxt);
	cxt.events.push_back(e);
	p.process(cxt);
	AST_EQ(0, cxt.userCj.size());
    }

    void test_money_earned_get_cj( void )
    {
	auto &cfg = SlotsConfig::getInstance().cjConfig;
	auto &x = cfg[EGE_EARNED_INCR];
	x.clear();
	x.push_back(createCjSetting(36, 500));
	x.push_back(createCjSetting(38, 1000));
	x.push_back(createCjSetting(37, 800));
	x.push_back(createCjSetting(35, 100));

	EventInfo e(EGE_EARNED_INCR, 50, 100);
	AchievementProcessor p;
	GameContext cxt;
	createGameContext(cxt);
	cxt.events.push_back(e);
	p.process(cxt);
	AST_EQ(1, cxt.userCj.size());
	AST_EQ(false, cxt.userCj[0].isRecvReward);
	AST_EQ(9, cxt.userCj[0].uid);
	AST_EQ(35, cxt.userCj[0].aid);
    }

    void test_money_earned_no_cj( void )
    {
	auto &cfg = SlotsConfig::getInstance().cjConfig;
	auto &x = cfg[EGE_EARNED_INCR];
	x.clear();
	x.push_back(createCjSetting(36, 500));
	x.push_back(createCjSetting(38, 1000));
	x.push_back(createCjSetting(37, 800));
	x.push_back(createCjSetting(35, 100));

	EventInfo e(EGE_EARNED_INCR, 50, 99);
	AchievementProcessor p;
	GameContext cxt;
	createGameContext(cxt);
	cxt.events.push_back(e);
	p.process(cxt);
	AST_EQ(0, cxt.userCj.size());
    }

    void test_money_earned_no_cj_pre_has_cj( void )
    {
	auto &cfg = SlotsConfig::getInstance().cjConfig;
	auto &x = cfg[EGE_EARNED_INCR];
	x.clear();
	x.push_back(createCjSetting(36, 500));
	x.push_back(createCjSetting(38, 1000));
	x.push_back(createCjSetting(37, 800));
	x.push_back(createCjSetting(35, 100));

	EventInfo e(EGE_EARNED_INCR, 100, 300);
	AchievementProcessor p;
	GameContext cxt;
	createGameContext(cxt);
	cxt.events.push_back(e);
	p.process(cxt);
	AST_EQ(0, cxt.userCj.size());
    }

    void test_money_earned_get_multi_cj( void )
    {
	auto &cfg = SlotsConfig::getInstance().cjConfig;
	auto &x = cfg[EGE_EARNED_INCR];
	x.clear();
	x.push_back(createCjSetting(36, 500));
	x.push_back(createCjSetting(38, 1000));
	x.push_back(createCjSetting(37, 800));
	x.push_back(createCjSetting(35, 100));

	EventInfo e(EGE_EARNED_INCR, 100, 800);
	AchievementProcessor p;
	GameContext cxt;
	createGameContext(cxt);
	cxt.events.push_back(e);
	p.process(cxt);
	AST_EQ(2, cxt.userCj.size());
	AST_EQ(false, cxt.userCj[0].isRecvReward);
	AST_EQ(9, cxt.userCj[0].uid);
	AST_EQ(36, cxt.userCj[0].aid);

	AST_EQ(false, cxt.userCj[1].isRecvReward);
	AST_EQ(9, cxt.userCj[1].uid);
	AST_EQ(37, cxt.userCj[1].aid);
    }

    void test_money_earned_get_last_cj( void )
    {
	auto &cfg = SlotsConfig::getInstance().cjConfig;
	auto &x = cfg[EGE_EARNED_INCR];
	x.clear();
	x.push_back(createCjSetting(36, 500));
	x.push_back(createCjSetting(38, 1000));
	x.push_back(createCjSetting(37, 800));
	x.push_back(createCjSetting(35, 100));

	EventInfo e(EGE_EARNED_INCR, 800, 10300);
	AchievementProcessor p;
	GameContext cxt;
	createGameContext(cxt);
	cxt.events.push_back(e);
	p.process(cxt);
	AST_EQ(1, cxt.userCj.size());
	AST_EQ(false, cxt.userCj[0].isRecvReward);
	AST_EQ(9, cxt.userCj[0].uid);
	AST_EQ(38, cxt.userCj[0].aid);
    }
};
