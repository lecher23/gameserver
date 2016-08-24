#include <cxxtest/TestSuite.h>
#include <string>
#include <thread>
#include <string.h>
#include <stdlib.h>
#include <resourcefactory.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;

class TestTimer : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void f(asio_service *service, asio_work * work, int count) {
	std::cout << count << " start." << std::endl;
	asio_error err;
	if (count == 3) {
	    delete work;
	} else {
	    work = new asio_work(*service);	    
	    service->run(err);
	}
	std::cout << err << std::endl;	
    }

    void test_simple( void )
    {
	auto &x = ResourceFactory::getInstance();
	x.init();
	auto &service = x.getIoService();
	asio_work *work = NULL;
	std::thread t1(std::bind(&TestTimer::f, this, &service, work, 1));
	sleep(1);
	work = new asio_work(service);
	std::thread t2(std::bind(&TestTimer::f, this, &service, work, 2));
	std::thread t3(std::bind(&TestTimer::f, this, &service, work, 3));
	t1.join();
	t2.join();
	t3.join();
    }
};
