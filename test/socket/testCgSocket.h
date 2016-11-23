#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <socket/future.h>
#include <boost/bind.hpp>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
// using namespace slots;

class testCgSocket: public CxxTest::TestSuite 
{
public:
    testCgSocket():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void handleAccept(std::shared_ptr<ConnPub> s, const asio_error &err) {
            s->start();
    }

    void callback(int32_t x, ConnPubPtr y) {
        std::cout << x << std::endl;
    }

    void test_main( void )
    {
        //code
        asio_service service;
        std::shared_ptr<ConnPub> socket(new ConnPub(service));
        std::shared_ptr<SyncWatcher<ConnPub>> sync(new SyncWatcher<ConnPub>());
        std::shared_ptr<UploadWatcher<ConnPub>> upload(new UploadWatcher<ConnPub>());
        socket->addWatcher(upload, sync);
        socket->setEstablishCallback(std::bind(&testCgSocket::callback, this, std::placeholders::_1, std::placeholders::_2));
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
