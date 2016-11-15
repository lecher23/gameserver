#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <boost/network.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <boost/network/protocol/http/response.hpp>


#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

namespace http = boost::network::http;


struct handler;

typedef boost::network::http::server<handler> http_server;

struct handler{
    void operator() (http_server::request const &request,
                     http_server::connection_ptr conn) {
        std::cout << request.destination<< std::endl;
        std::cout << request.body<< std::endl;
        std::string out="hello";
        conn->write(out);
    }

    void log(http_server::string_type const &info) {
        std::cerr << "ERROR:" << info << "\n";
    }
};


class testCppNetlib: public CxxTest::TestSuite 
{
public:
    testCppNetlib():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        handler handler_;
        http_server::options options(handler_);
        http_server server_(
            options.address("0.0.0.0").port("34667"));
        server_.run();
    }
private:
    bool _inited;
};
