#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysqlconnpool.h>
#include <util/config.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define private, public
#include <slots/slotsconfig.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string config_file = cxx::cfg_file;

class TestSlotsConfigNoMysql : public CxxTest::TestSuite
{
public:
TestSlotsConfigNoMysql():_inited(false){}
  virtual void setUp(){
  }

  virtual void tearDown() {
  }

  void test_earned() {
    GameContext context;
    SlotsUserPtr su(new SlotsUser);
    su->uRes.vipLevel = 1;
    context.user = su;

    auto &cfg = SlotsConfig::getInstance().vipSetting;
    cfg[1].bounus_ext = 0.3;

    auto result = SlotsConfig::getInstance().earned(context, 100);
    ast_eq(130, result);
  }

  void test_earned_level_0() {
    GameContext context;
    SlotsUserPtr su(new SlotsUser);
    su->uRes.vipLevel = 0;
    context.user = su;

    auto &cfg = SlotsConfig::getInstance().vipSetting;
    cfg[1].bounus_ext = 0.3;

    auto result = SlotsConfig::getInstance().earned(context, 100);
    ast_eq(100, result);
  }

  void test_expGain() {
    GameContext context;
    SlotsUserPtr su(new SlotsUser);
    su->uRes.vipLevel = 1;
    context.user = su;

    auto &cfg = SlotsConfig::getInstance();
    cfg.vipSetting[1].exp_ext = 0.5;
    cfg.bet2Exp[100] = 200;

    auto result = SlotsConfig::getInstance().expGain(context, 100);
    ast_eq(300, result);
  }

  void test_getGridConfig() {
    GridConfigs gc;
    for(int i = 0; i < 5; ++i) {
      GridConfig cfg;
      cfg.gridIdx = i;
      cfg.weight = i * 5;
    }
  }

private:
  bool _inited;
};
