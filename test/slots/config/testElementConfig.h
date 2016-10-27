#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/config/sloteleconfig.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testElementConfig: public CxxTest::TestSuite 
{
public:
    testElementConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
        SlotEleConfig cfg;
        ast_true(cfg.initFromJsonFile("config_files/ElementConfigTest.json"));
        ast_eq(12, cfg.end());
        const auto &item = cfg.getElement(2);
        ast_eq(1003, item.id);
        ast_eq(3, item.type);
        ast_eq(false, item.repeatable);
        const auto &item2 = cfg.getElement(7);
        ast_eq(1008, item2.id);
        ast_eq(4, item2.reward.size());
        // ast_eq(3, item2.reward[2]);
        // ast_eq(15, item2.reward[3]);
        // ast_eq(30, item2.reward[4]);
        // ast_eq(200, item2.reward[5]);
    }
private:
    bool _inited;
};
