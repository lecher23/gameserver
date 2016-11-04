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
        std::thread t0(
            std::bind(&testProducerConsumerQueue::consumer_helper, this, &pcq, 4));
        std::thread t1(
            std::bind(&testProducerConsumerQueue::consumer_helper, this, &pcq, 3));
        std::thread t2(
            std::bind(&testProducerConsumerQueue::producer_helper, this, &pcq, 1));
        std::thread t3(
            std::bind(&testProducerConsumerQueue::producer_helper, this, &pcq, 2));
        std::thread t4(
            std::bind(&testProducerConsumerQueue::consumer_helper, this, &pcq, 5));
        std::thread t5(
            std::bind(&testProducerConsumerQueue::consumer_helper, this, &pcq, 6));
        t0.join();
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
    }

    void consumer_helper(ProducerConsumerQueue<std::string> *q, int id) {
        while (true) {
            const auto &product = q->consumer();
            CLOG(INFO) << "Producer " << id << " Get product:" << product;
        }
    }

    void producer_helper(ProducerConsumerQueue<std::string> *q, int id) {
        std::string s = "123";
        while(true){
            CLOG(INFO) << id << " Add product";
            q->produce(s);
            sleep(id);
        }
    }

private:
    bool _inited;
};
