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
        CgTcpProtol p(buffer);
        for (size_t i = 0; i < MSG_HEAD_LEN; ++i) {
            p._header[i] = '1';
        }
        ast_true(p.getBodyLen());
        ast_eq(1111, p._bytesExpected);
    }

    void test_getBodyLen_failed(void) {
        DataBuffer buffer;
        CgTcpProtol p(buffer);
        for (size_t i = 0; i < MSG_HEAD_LEN; ++i) {
            p._header[i] = '1';
        }
        p._header[0] = '-';
        ast_true(!p.getBodyLen());
    }
private:
    bool _inited;
};
