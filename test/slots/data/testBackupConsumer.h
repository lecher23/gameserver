#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "slots/data/backupconsumer.h"
#include "util/producerconsumerqueue.h"
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testBackupConsumer: public CxxTest::TestSuite 
{
public:
    testBackupConsumer():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_stop_no_product( void )
    {
        cgserver::ProducerConsumerQueue<std::string> queue;
        BackupConsumer bc(queue);
        bc.start();
        sleep(1);
        bc.stop();
    }
private:
    bool _inited;
};
