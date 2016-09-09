#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <servicethread.h>
#include <servertimer.h>
#include <util/timeutil.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;

class TestServiceThread : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_simple( void )
    {
	asio_service s;
	ServiceThread t(s);
	t.start();
	sleep(1);
	ast_true(t.isStart());
	ServerTimer st(s);
	std::cout << "Now:" << CTimeUtil::getCurrentTimeInSeconds() << std::endl;
	st.addTask([](){
		std::cout << "Timer works:" << CTimeUtil::getCurrentTimeInSeconds() << std::endl;
	    },
	    ServerTimer::EET_10_S);
	st.addTask([](){
		std::cout << "Timer works:" << CTimeUtil::getCurrentTimeInSeconds() << std::endl;
	    },
	    ServerTimer::EET_10_S);
	st.addTask([](){
		std::cout << "Timer works:" << CTimeUtil::getCurrentTimeInSeconds() << std::endl;
	    },
	    ServerTimer::EET_10_S);
	st.addTask([](){
		std::cout << "Timer works:" << CTimeUtil::getCurrentTimeInSeconds() << std::endl;
	    },
	    ServerTimer::EET_10_S);
	std::cout << "Now:" << CTimeUtil::getCurrentTimeInSeconds() << std::endl;	
	t.stop();
	sleep(20);
	ast_true(!t.isStart());
    }
};
