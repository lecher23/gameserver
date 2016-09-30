#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define private public
#include <slots/themes/themeconfig.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testThemeConfig: public CxxTest::TestSuite 
{
public:
    testThemeConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_getCommonConfig()
    {
      ThemeCommonConfig tcc;
      ThemeCommonConfigItem tcci;
    }
private:
    bool _inited;
};
