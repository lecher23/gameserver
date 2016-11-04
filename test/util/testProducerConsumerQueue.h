#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include "util/producerconsumerqueue.h"
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testProducerConsumerQueue: public CxxTest::TestSuite 
{
public:
    testProducerConsumerQueue():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_main( void )
    {
        ProducerConsumerQueue<std::string> pcq;
        pcq.setCapacity(10);
        std::thread t1(
            std::bind(&testProducerConsumerQueue::consumer_helper, this, &pcq, 4));
        std::thread t2(
            std::bind(&testProducerConsumerQueue::producer_helper, this, &pcq, 1));
        std::thread t3(
            std::bind(&testProducerConsumerQueue::producer_helper, this, &pcq, 2));
        t1.join();
        t2.join();
        t3.join();
    }

    void consumer_helper(ProducerConsumerQueue<std::string> *q, int size) {
        for (int i = 0; i < size; ++i) {
            const auto &product = q->consumer();
            CLOG(INFO) << "Get product:" << product;
            const char *p = &product[0];
            printf("%p\n", p);
            sleep(0.5);
        }
    }

    void producer_helper(ProducerConsumerQueue<std::string> *q, int id) {
        std::string s = "123";
        for (int i = 0; i< 10; ++i) {
            CLOG(INFO) << id << " Add product";
            q->produce(s);
        }
    }

private:
    bool _inited;
};
