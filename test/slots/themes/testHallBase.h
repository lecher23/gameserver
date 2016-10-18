#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/themes/hallbase.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testHallBase: public CxxTest::TestSuite 
{
public:
    testHallBase():_inited(false){
        if (!_inited) {
            ast_true(RedisClientFactory::getClient().Initialize(
                         "127.0.0.1", 6379, 2, 3));
            _inited = true;
        }
    }

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_operateHallPrize( void )
    {
        auto &client = RedisClientFactory::getClient();
        ast_eq(RC_SUCCESS, client.Zadd("H:prz", 1000, "1"));

        HallBase hb;
        ast_eq(1000, hb.getHallPrize(1));
        ast_eq(1999, hb.incrHallPrize(1, 999));
    }

    void test_operateHallPrize( void )
    {
        auto &client = RedisClientFactory::getClient();
        ast_eq(RC_SUCCESS, client.Zadd("H:prz", 1000, "1"));

        HallBase hb;
        ast_eq(1000, hb.getHallPrize(1));
        ast_eq(1999, hb.incrHallPrize(1, 999));
    }
private:
    bool _inited;
};
