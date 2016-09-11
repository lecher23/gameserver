#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#define private public
#include <socket/cgtcpprotol.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
// using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testCgTcpProtol: public CxxTest::TestSuite 
{
public:
    testCgTcpProtol():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_getBodyLen_success( void )
    {
        DataBuffer buffer;
        CgTcpProtol p;
        for (size_t i = 0; i < MSG_HEAD_LEN; ++i) {
            p._header[i] = '1';
        }
        ast_true(p.getBodyLen());
        ast_eq(1111, p._left);
    }

    void test_getBodyLen_failed(void) {
        DataBuffer buffer;
        CgTcpProtol p;
        for (size_t i = 0; i < MSG_HEAD_LEN; ++i) {
            p._header[i] = '1';
        }
        p._header[0] = '-';
        ast_true(!p.getBodyLen());
    }

    void test_parseHead_empty(void) {
        DataBuffer buffer;
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[1];
        ast_true(p.parseHead(x, 0));
    }

    void test_parseHead_partial(void) {
        DataBuffer buffer;
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[] = {"13"};
        int32_t len = 2;
        ast_true(p.parseHead(x, len));
        ast_eq(2, p._left);
        ast_eq('1', p._header[0]);
        ast_eq('3', p._header[1]);
        ast_eq(0, p._packets.size());
        ast_eq(CgTcpProtol::EPS_HEAD, p._step);
    }

    void test_parseHead_full(void) {
        DataBuffer buffer;
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[] = {"1313"};
        int32_t len = sizeof(x) - 1;
        ast_true(p.parseHead(x, len));
        ast_eq(1313, p._left);
        ast_eq('1', p._header[0]);
        ast_eq('3', p._header[1]);
        ast_eq(0, p._packets.size());
        ast_eq(CgTcpProtol::EPS_BODY, p._step);
    }

    void test_parseHead_with_partial_body(void) {
        DataBuffer buffer;
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[] = {"0004123"};
        int32_t len = sizeof(x) - 1;
        ast_true(p.parseHead(x, len));
        ast_eq(1, p._left);
        ast_eq(0, p._packets.size());
        ast_eq(CgTcpProtol::EPS_BODY, p._step);
    }

    void test_parseHead_with_full_body(void) {
        DataBuffer buffer;
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[] = {"00041234"};
        int32_t len = sizeof(x) - 1;
        ast_true(p.parseHead(x, len));
        ast_eq(4, p._left);
        ast_eq(1, p._packets.size());
        ast_eq(CgTcpProtol::EPS_HEAD, p._step);
    }

    void test_parseHead_with_multi_packet(void) {
        DataBuffer buffer;
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[] = {"0002120003123000412"};
        int32_t len = sizeof(x) - 1;
        ast_true(p.parseHead(x, len));
        ast_eq(2, p._left);
        ast_eq(2, p._packets.size());
        ast_eq(CgTcpProtol::EPS_BODY, p._step);
        auto ret = p.nextPacket();
        ast_eq(2, ret.size());
        ast_eq("12", ret);
        ret = p.nextPacket();
        ast_eq(3, ret.size());
        ast_eq("123", ret);
    }

    void test_parse_with_clear_start() {
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[] = {"000212"};
        int32_t len = sizeof(x) - 1;
        ast_true(p.parse(x, len));
        ast_eq(4, p._left);
        ast_eq(1, p._packets.size());
        ast_eq(CgTcpProtol::EPS_HEAD, p._step);
        auto ret = p.nextPacket();
        ast_eq(2, ret.size());
        ast_eq("12", ret);
    }

    void test_parse_with_partial() {
        CgTcpProtol p;
        ast_eq(0, p._packets.size());
        char x[] = {"12300021"};
        int32_t len = sizeof(x) - 1;
        p._step = CgTcpProtol::EPS_BODY;
        p._left = 3;
        p._curPacket = "456";
        ast_true(p.parse(x, len));
        ast_eq(1, p._left);
        ast_eq(1, p._packets.size());
        ast_eq(CgTcpProtol::EPS_BODY, p._step);
        auto ret = p.nextPacket();
        ast_eq(6, ret.size());
        ast_eq("456123", ret);
    }

private:
    bool _inited;
};
