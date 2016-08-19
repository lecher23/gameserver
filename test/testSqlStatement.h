#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#include "mysql/sqlstatement.h"
#include "asyncserver.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;
class TestSqlStatement : public CxxTest::TestSuite 
{
public:
    void test_hasCondition( void )
    {
	std::cout << "test has condition" << std::endl;
	std::string out;
	SqlStatement ss(out);
	ss.hasCondition();
	AST_EQ(" where ", out);
    }

    void test_setCondition( void )
    {
	std::string out;
	SqlStatement ss(out);
	ss.setCondition("uid = 3");
	AST_EQ(" where uid = 3", out);
    }

    void test_addEqualCondition( void )
    {
	std::string out;
	SqlStatement ss(out);
	ss.addEqualCondition("uid", "3");
	AST_EQ(" where (uid=\"3\")", out);
    }

    void test_complicate( void )
    {
	std::string out;
	SqlStatement ss(out);
	ss.hasCondition();
	ss.addEqualCondition("uid", "3");
	ss.setConditionJoin(true);
	ss.setCondition("mid=3");
	ss.setOrderField("price", true);
	ss.setLimit("3", "9");
	AST_EQ(" where (uid=\"3\") and mid=3 order by price asc  limit 3,9", out);
    }

    void test_innerJoin(void)
    {
	std::string out;
	SqlStatement ss(out);
	ss.innerJoin("t1", "t2", "id1", "id2");
	AST_EQ(" from t1 inner join t2 on t1.id1=t2.id2", out);
    }

    void test_innerJoinWithAnd(void)
    {
	std::string out;
	SqlStatement ss(out);
	ss.innerJoin("t1", "t2", "id1", "id2");
	ss.setConditionJoin(true);
	ss.addEqualCondition("t1.id1", "123");
	AST_EQ(" from t1 inner join t2 on t1.id1=t2.id2 and (t1.id1=\"123\")", out);
    }
    
};
