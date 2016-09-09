#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "util/requestvalidator.h"
#include "http/httppacket.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;

const std::string lua_file = "/home/licheng/workspace/lua/main.lua";

class MyTestSuite : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_simple( void )
    {
	std::string secret = "xO8v9a";
	HTTPPacket packet;
	packet.setURI("uid=12345&ts=1231314123&sign=99d14679260b1883ce2c6d30b01682e9");
	packet.addParam("uid", "12345");
	packet.addParam("ts", "14708148548801234");
	packet.addParam("sign", "99d14679260b1883ce2c6d30b01682e9");

	RequestValidator v;
	AST_TRUE(v.validate(packet, secret));
    }
};
