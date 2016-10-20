#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/resultformatter.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testResultFormatter: public CxxTest::TestSuite 
{
public:
    testResultFormatter():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_formatHallList_2vec_size_equal()
    {
        SBuf buf;
        ResultFormatter rf(buf);
        std::vector<std::string> a;
        a.push_back(std::string("room1"));
        a.push_back(std::string("room2"));
        std::vector<std::string> b;
        b.push_back(std::string("100"));
        b.push_back(std::string("23"));
        ast_eq(2, a.size());
        ast_eq(2, b.size());
        rf.formatHallList(a, b);
        std::string expected = "{\"rl\":[{\"id\":\"room1\",\"pz\":\"100\"},{\"id\":\"room2\",\"pz\":\"23\"}],\"st\":\"OK\"}";
        ast_eq(expected, buf.GetString());
    }
    void test_formatHallList_2vec_size_not_eqal()
    {
        SBuf buf;
        ResultFormatter rf(buf);
        std::vector<std::string> a;
        a.push_back(std::string("room1"));
        a.push_back(std::string("room2"));
        std::vector<std::string> b;
        b.push_back(std::string("23"));
        ast_eq(2, a.size());
        ast_eq(1, b.size());
        rf.formatHallList(a, b);
        ast_eq('\0', *(buf.GetString()));
    }
private:
    bool _inited;
};
