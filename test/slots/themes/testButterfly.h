#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#include <slots/themes/butterfly.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testButterfly: public CxxTest::TestSuite 
{
public:
    testButterfly():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        //code
        Butterfly bf;
        bf.addFactor(2);
        bf.addFactor(10);
        bf.addFactor(15);
        bf.addFactor(25);
        bf.addFactor(30);
        bf.addFactor(35);

        std::vector<int32_t> ret;
        int32_t multiple;
        bf.play(3, multiple, ret);
        ast_eq(3, ret.size());
        CLOG(INFO) << "Mutiple:" << multiple;
        for (auto x: ret) {
            CLOG(INFO) << x;
        }
    }
private:
    bool _inited;
};
