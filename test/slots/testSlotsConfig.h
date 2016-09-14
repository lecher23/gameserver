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

private:
  bool _inited;
};
