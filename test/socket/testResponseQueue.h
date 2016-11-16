#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <socket/responsequeue.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
// using namespace slots;

class testResponseQueue: public CxxTest::TestSuite 
{
public:
    testResponseQueue():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        ResponseQueue<std::string> rq(3);
        ast_true(rq.empty());
        rq.push_back("1");
        ast_true(!rq.empty());
        rq.push_back("2");
        ast_true(!rq.empty());
        ast_eq("1", rq.front());
        rq.push_back("3");
        ast_eq("2", rq.pop_front());
        ast_eq("3", rq.pop_front());
        ast_true(rq.empty());
    }
private:
    bool _inited;
};
