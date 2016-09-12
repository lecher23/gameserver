#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#include <socket/cgsocket.h>
#include <boost/bind.hpp>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
// using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testCgSocket: public CxxTest::TestSuite 
{
public:
    testCgSocket():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void handleAccept(std::shared_ptr<CgSocket> s, const asio_error &err) {
            s->start();
    }

    void test_main( void )
    {
        //code
        asio_service service;
        std::shared_ptr<CgSocket> socket(new CgSocket(service));
        boost::asio::ip::tcp::acceptor
            acceptor(service, boost::asio::ip::tcp::endpoint(
                         boost::asio::ip::tcp::v4(), 9888), true);
        acceptor.async_accept(socket->getSocket(),
            boost::bind(&testCgSocket::handleAccept,
                    this, socket, asio_placeholders::error));
        service.run();
    }
private:
    bool _inited;
};
