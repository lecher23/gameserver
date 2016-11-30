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
        ast_true(handler.parseURI("/slots/login?u82&mid=licheng&a=b&uid=99", packet));
        std::string out;
        ast_true(packet.getParamValue("mid", out));
        ast_eq("licheng", out);
        ast_true(packet.getParamValue("uid", out));
        ast_eq("99", out);
        ast_true(handler.parseURI("/slots/login", packet));
        ast_true(handler.parseURI("/slots/login?key=val", packet));
        ast_true(packet.getParamValue("key", out));
        ast_eq("val", out);
    }
private:
    bool _inited;
};
