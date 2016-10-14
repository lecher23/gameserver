#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

#include <cache/redisclientfactory.h>

using namespace cgserver;
// using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testRedisClient: public CxxTest::TestSuite 
{
public:
    testRedisClient():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        auto &client = RedisClientFactory::getClient();
        if (!client.Initialize("127.0.0.1", 6379, 2, 3)) {
            CLOG(ERROR) << "connect to redis failed";
            ast_true(false);
            return;
        }
        ast_eq(0, client.Hset("licheng", "age", "12"));
        std::string val;
        ast_eq(0, client.Hget("licheng", "age", &val));
        ast_eq("12", val);
    }
private:
    bool _inited;
};
