#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/config/vipconfig.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testVipConfig: public CxxTest::TestSuite 
{
public:
    testVipConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
        VipConfig vc;
        ast_true(vc.initFromJsonFile("config_files/VipConfigTest.json"));
        const auto& ret = vc.getVipConfigInfo(5);
        ast_eq(5, ret.level);
        ast_true(ret.expExtra > 0.299999 && ret.expExtra < 0.300001)
        ast_eq(400, ret.pointNeed);
        ast_eq(30, ret.maxBet);
    }
private:
    bool _inited;
};
