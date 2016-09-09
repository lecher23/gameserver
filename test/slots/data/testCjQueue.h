#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/data/cjqueue.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

class TestCjQueue : public CxxTest::TestSuite 
{
public:
    TestCjQueue():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_swap_queue( void )
    {
            CjQueue q;
            ast_eq(0, q._curWrite);
            ast_eq(1, q._curRead);
            q.swapQueue();
            ast_eq(1, q._curWrite);
            ast_eq(0, q._curRead);
    }

    void test_get_sqls_empty(void)
    {
            CjQueue q;
            ast_true(q._queue[q._curRead].empty());
            std::vector<std::string> out;
            q.getSqls(out);
            ast_true(out.empty());
    }

    void test_get_sqls_single(void)
    {
            CjQueue q;
            ast_true(q._queue[q._curRead].empty());
            UserCJ cj;
            cj.uid = "1";
            cj.aid = "2";
            cj.isRecvReward = false;
            cj.progress = 100;
            cj.isGain = false;
            cj.time = 199012312;
            std::vector<UserCJ> in;
            in.push_back(cj);
            q.addCj(in);
            q.swapQueue();
            std::vector<std::string> out;
            q.getSqls(out);
            ast_eq(2, out.size());
            ast_eq("DELETE FROM user_achievement WHERE uid=1 AND aid=2", out[0]);
            ast_eq("INSERT INTO user_achievement ( uid,aid,is_recv_reward,progress,is_gain,time ) VALUES ( \"1\",\"2\",\"0\",\"100\",\"0\",\"199012312\" ) ", out[1]);
    }

    void test_get_sqls_multi(void)
    {
            CjQueue q;
            ast_true(q._queue[q._curRead].empty());

            UserCJ cj;
            cj.uid = "1";
            cj.aid = "2";
            cj.isRecvReward = false;
            cj.progress = 100;
            cj.isGain = false;
            cj.time = 199012312;
            std::vector<UserCJ> in;
            in.push_back(cj);

            cj.uid = "1";
            cj.aid = "6";
            cj.isRecvReward = false;
            cj.progress = 6;
            cj.isGain = true;
            cj.time = 2048;
            in.push_back(cj);

            q.addCj(in);
            q.swapQueue();
            std::vector<std::string> out;
            q.getSqls(out);
            ast_eq(4, out.size());
            ast_eq("DELETE FROM user_achievement WHERE uid=1 AND aid=2", out[0]);
            ast_eq("INSERT INTO user_achievement ( uid,aid,is_recv_reward,progress,is_gain,time ) VALUES ( \"1\",\"2\",\"0\",\"100\",\"0\",\"199012312\" ) ", out[1]);
            ast_eq("DELETE FROM user_achievement WHERE uid=1 AND aid=6", out[2]);
            ast_eq("INSERT INTO user_achievement ( uid,aid,is_recv_reward,progress,is_gain,time ) VALUES ( \"1\",\"6\",\"0\",\"6\",\"1\",\"2048\" ) ", out[3]);
    }

private:
    bool _inited;
};
