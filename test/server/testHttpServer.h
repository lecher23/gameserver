#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <server/httpserver.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;

class testHttpServer: public CxxTest::TestSuite 
{
public:
    testHttpServer():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_httphandler_parseURI( void )
    {
        HttpHandler handler;
        HTTPPacket packet;
        ast_true(handler.parseURI("/slots/login?mid=licheng&a=b", packet));
        std::string out;
        ast_true(packet.getParamValue("mid", out));
        ast_eq("licheng", out);
    }
private:
    bool _inited;
};
