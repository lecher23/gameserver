#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/config/onlineconfig.h>
#undef private
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testOnlineConfig: public CxxTest::TestSuite 
{
public:
    testOnlineConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_parse( void )
    {
        OnlineConfig oc;
        ast_true(oc.initFromJsonFile("config_files/OnlineConfigTest.json"));
        ast_eq(5, oc._cfg.size());
        ast_eq(12, oc._runner.size());
        ast_eq(85, oc._runnerTotalWeight);
    }

    void test_next_level_start_with_0(){
        OnlineConfig oc;
        ast_true(oc.initFromJsonFile("config_files/OnlineConfigTest.json"));
        int32_t timeNeed = 0;
        int64_t reward = 0;
        ast_eq(1, oc.nextLevel(0, 0, timeNeed, reward));
        ast_eq(1200, timeNeed);
        ast_eq(500, reward);
    }

    void test_next_level_cur_is_1(){
        OnlineConfig oc;
        ast_true(oc.initFromJsonFile("config_files/OnlineConfigTest.json"));
        int32_t timeNeed = 0;
        int64_t reward = 0;
        ast_eq(2, oc.nextLevel(1, 1200, timeNeed, reward));
        ast_eq(3600, timeNeed);
        ast_eq(500, reward);
    }

    void test_next_level_not_change() {
        OnlineConfig oc;
        ast_true(oc.initFromJsonFile("config_files/OnlineConfigTest.json"));
        int32_t timeNeed = 0;
        int64_t reward = 0;
        ast_eq(1, oc.nextLevel(1, 800, timeNeed, reward));
        ast_eq(400, timeNeed);
        ast_eq(0, reward);
    }

    void test_next_level_edge() {
        OnlineConfig oc;
        ast_true(oc.initFromJsonFile("config_files/OnlineConfigTest.json"));
        int32_t timeNeed = 0;
        int64_t reward = 0;
        oc._enableLoop = false;
        ast_eq(4, oc.nextLevel(3, 3700, timeNeed, reward));
        ast_eq(3600, timeNeed);
        ast_eq(1000, reward);
    }

    void test_next_level_end_no_loop() {
        OnlineConfig oc;
        ast_true(oc.initFromJsonFile("config_files/OnlineConfigTest.json"));
        oc._enableLoop = false;
        int32_t timeNeed = 0;
        int64_t reward = 0;
        ast_eq(5, oc.nextLevel(4, 3700, timeNeed, reward));
        ast_eq(86400, timeNeed);
    }

    void test_next_level_end_loop() {
        OnlineConfig oc;
        ast_true(oc.initFromJsonFile("config_files/OnlineConfigTest.json"));
        oc._enableLoop = true;
        int32_t timeNeed = 0;
        int64_t reward = 0;
        ast_eq(0, oc.nextLevel(4, 3700, timeNeed, reward));
        ast_eq(0, timeNeed);
    }

private:
    bool _inited;
};
