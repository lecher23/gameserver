#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/themes/butterfly.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

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
        CLOG(INFO) << "Mutiple:" << multiple;
        for (auto x: ret) {
            CLOG(INFO) << x;
        }
    }

    void test_initfromjsonfile() {
        Butterfly bf;
        ast_true(bf.initFromJsonFile("config_files/butterfly.json"));
        ast_eq(1, bf._id);
        ast_eq(10, bf.factors.size());
        ast_eq(2, bf.factors[0]);
        ast_eq(24, bf.factors[2]);
        ast_eq(6, bf.factors[3]);
        ast_eq(12, bf.factors[9]);
    }
private:
    bool _inited;
};
