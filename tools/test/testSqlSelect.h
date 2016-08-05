#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "mysql/sqlselect.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y)
#define AST_TRUE(x) TS_ASSERT(x)

using namespace cgserver;
class TestSqlSelect : public CxxTest::TestSuite 
{
public:
    void test_sigle_field_without_cond( void )
    {
	std::string out;
        SqlSelect ss(out);
	ss.addField("f1");
	ss.addTable("t1");
	AST_EQ("select f1 from t1", out)
    }
    
    void test_multi_field( void )
    {
	std::string out;
        SqlSelect ss(out);
	ss.addField("f1");
	ss.addField("f2");
	ss.addTable("t1");
	AST_EQ("select f1,f2 from t1", out)
    }

    void test_multi_table( void )
    {
	std::string out;
        SqlSelect ss(out);
	ss.addField("f1");
	ss.addField("f2");
	ss.addTable("t1");
	ss.addTable("t2");
	AST_EQ("select f1,f2 from t1,t2", out)
    }

    void test_no_filed( void )
    {
	std::string out;
        SqlSelect ss(out);
	ss.addTable("t1");
	ss.addTable("t2");
	AST_EQ("select *  from t1,t2", out)
    }

    void test_no_table( void )
    {
	std::string out;
        SqlSelect ss(out);
	AST_EQ("", out)
    }
    
};
