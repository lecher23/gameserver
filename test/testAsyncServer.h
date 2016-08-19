#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "asyncserver.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;
class TestAsyncServer : public CxxTest::TestSuite 
{
public:
    void test_simple(void)
    {
	std::cout << "\n Begin"<< std::endl;	
	asio_service service;
	std::cout << "service"<< std::endl;
        AsyncServer server(service, 9877);
	server.start();
    }
};