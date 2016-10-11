#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/config/cjconfig.h>
#undef private
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testCjConfig: public CxxTest::TestSuite 
{
public:
    testCjConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        CjConfig cc;
        ast_true(cc.initFromJsonFile("config_files/CjConfigTest.json"));

        const auto &ret = cc.getCjInfo(3);
        ast_eq(3, ret.cjID);
        ast_eq(0, ret.hallID);
        ast_eq(15, ret.target);
        ast_eq(3000, ret.rewardValue);
        ast_eq(8, cc.config[6].size());

        std::vector<int32_t> ids;
        cc.getCjID(6, 1, 5, 20, ids);
        ast_eq(3, ids.size());
        ast_eq(2, ids[0]);
        ast_eq(3, ids[1]);
        ast_eq(4, ids[2]);

        cc.getCjID(10, 2, 9, 10, ids);
        ast_eq(0, ids.size());
    }
private:
    bool _inited;
};
