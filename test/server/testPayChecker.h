#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <server/paychecker.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;

class TestPayChecker : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_start_stop( void )
    {
            PayChecker pc(2000, 3);
            bool _saved = false;
            bool _queried = false;
            std::string qid = "";
            pc.setSaveFunc([&](PayInfo * p){
                            CLOG(INFO) << "save..." ;
                            _saved = true;
                            return true;});
            pc.setAsyncCheckFunc([&](asio_service *s, PayInfo *p)
                                 {
                                         CLOG(INFO) << "async check.";
                                         _queried = true;
                                         qid = p->id;
                                 });
            pc.start();
            auto &x = pc.getPayData("123");
            ast_eq("123", qid);
            ast_eq("123", x.id);
            sleep(10);
            pc.stop();
            ast_true(_saved);
            ast_true(_queried);
    }

private:
};
