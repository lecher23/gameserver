#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/config/loginsetting.h>
#undef private
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testLoginSetting: public CxxTest::TestSuite 
{
public:
    testLoginSetting():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_init( void )
    {
        //code
        LoginSetting ls;
        ast_true(ls.initFromJsonFile("config_files/LoginSettingTest.json"));
        ast_eq(12, ls._runnerReward.size());
        ast_eq(6000, ls._runnerReward[2].reward);
        ast_eq(10, ls._runnerReward[2].weight);
        ast_eq(7, ls._dayReward.size());
        ast_eq(10000, ls._dayReward[7]);
    }
private:
    bool _inited;
};
