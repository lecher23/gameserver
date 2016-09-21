#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

#include <hiredis/hiredis.h>

// using namespace cgserver;
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
      const char *hostname = "139.196.148.39";
      int port = 6379;
      struct timeval timeout = {1, 50000};
      redisContext *c;
      redisReply *reply;
      c = redisConnectWithTimeout(hostname, port, timeout);
      if (c == nullptr || c->err) {
        if (c) {
          printf("Connection error: %s\n", c->errstr);
          redisFree(c);
        } else {
          printf("Connection error: can't allocate redis context\n");
        }
        ast_true(false);
        return;
      }
      std::string strPing("PING");
      reply = (redisReply *)redisCommand(c,"PING");
      printf("PING: %s\n", reply->str);
      freeReplyObject(reply);

      reply = (redisReply *)redisCommand(c,"SET %s %s", "foo", "hello world");
      printf("SET: %s\n", reply->str);
      freeReplyObject(reply);
    }
private:
    bool _inited;
};
