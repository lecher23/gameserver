#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysqlconnpool.h>
#include <slots/slotsconfig.h>
#include <util/config.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string config_file = cxx::cfg_file;

class TestSlotsConfig : public CxxTest::TestSuite
{
public:
TestSlotsConfig():_inited(false){}
  virtual void setUp(){
    if (!_inited){
      ast_true(Config::getInstance().initConfig(config_file));
      ast_true(MysqlConnPool::getInstance().init());
      ast_true(SlotsConfig::getInstance().init());
      _inited = true;
    }
  }

  virtual void tearDown() {
  }

  void test_init_cargo_infos_success( void )
  {
    auto &cfg = SlotsConfig::getInstance().cargoInfo;
    ast_eq(1, cfg.size());
    if (cfg.size() > 0) {
      ast_eq(1000, cfg[0]->price);
    }
  }

  void test_init_level_info_success() {
    auto &cfg = SlotsConfig::getInstance().levelConfig;
    ast_eq(6, cfg.size());
    auto itr = cfg.find(4);
    ast_true(itr != cfg.end());
    ast_eq(1300, itr->second.expNeed);
  }

  void test_earned() {
    _inited = false;
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
    _inited = false;
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
    _inited = false;
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

private:
  bool _inited;
};
