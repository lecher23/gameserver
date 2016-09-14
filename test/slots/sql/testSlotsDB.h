#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/sql/slotsdb.h>
#undef private
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#include <util/config.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string cfg_file = cxx::cfg_file;

class SlotsDBTest : public CxxTest::TestSuite 
{
public:
SlotsDBTest():_inited(false){}

  virtual void setUp(){
    if (!_inited){
      ast_true(Config::getInstance().initConfig(cfg_file));
      ast_true(MysqlConnPool::getInstance().init());
      _inited = true;
    }
  }

  virtual void tearDown() {
  }

  void test_getUserInfoByMachineId(void)
  {
    std::string mid = "123456";

    MysqlSimpleSelect mss;
    mss.setField("*");
    mss.innerJoin(gUserInfo, gUserResource, "uid", "uid");
    mss.addCondition("mid", mid, true, true);
    CLOG(INFO) << mss.getQuery();

    SlotsUser su;
    ast_true(SlotsDB::getInstance().getUserInfoByMachineId(mid, su));
    ast_eq("123456", su.uInfo.mid);
    ast_eq("11", su.uInfo.uid);
    ast_eq("11", su.uRes.uid);
    ast_eq(3, su.uRes.level);
    ast_eq(2, su.uRes.vipLevel);
  }

private:
  bool _inited;
};
