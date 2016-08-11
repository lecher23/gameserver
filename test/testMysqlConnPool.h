#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "mysql/mysqlconnpool.h"
#include "mysql/mysqlsimpleselect.h"
#include "mysql/mysqlsimpledelete.h"
#include "mysql/mysqlsimpleinsert.h"
#include "mysql/mysqlsimpleupdate.h"
#include "util/config.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;

const std::string configFile = "/home/licheng/workspace/CgServerPlus/server.cfg";

class MysqlConnPoolTest : public CxxTest::TestSuite 
{
public:
 MysqlConnPoolTest():_inited(false){}

    virtual void setUp(){
	if (!_inited){
	    AST_TRUE(Config::getInstance().initConfig(configFile));
	    AST_TRUE(MysqlConnPool::getInstance().init());
	    _inited = true;
	}
    }

    virtual void tearDown() {
	MysqlSimpleDelete msd;
	msd.setTable("user_info");
	msd.setCondition("mid=\"test_update\"", false);
	MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msd);

	MysqlSimpleDelete msd1;
	msd1.setTable("user_info");
	msd1.setCondition("mid=\"test_delete\"", false);
	MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msd1);

	MysqlSimpleDelete msd2;
	msd2.setTable("user_info");
	msd2.setCondition("mid=\"test_add\"", false);
	MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msd2);

    }

    void test_select( void )
    {
	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.setTable("user_info");
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &mss));
	AST_TRUE(mss.result.size() > 0);
    }

    void test_delete( void )
    {

	MysqlSimpleInsert msi;
	msi.setTable("user_info");
	msi.setField("mid");
	msi.addField("male");
	msi.setValue("test_delete");
	msi.addValue("1");
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msi));
	
	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.setTable("user_info");
	mss.setCondition("mid", "test_delete", true);
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &mss));
	size_t total = mss.result.size();
	AST_TRUE(total > 0);

	MysqlSimpleDelete msd;
	msd.setTable("user_info");
	msd.setCondition("mid=\"test_delete\"", false);
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msd));

	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &mss));
	size_t total2 = mss.result.size();
	AST_TRUE(total2 == 0);
	
    }

    void test_update( void )
    {

	MysqlSimpleInsert msi;
	msi.setTable("user_info");
	msi.setField("mid");
	msi.addField("male");
	msi.setValue("test_update");
	msi.addValue("1");
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msi));
	
	MysqlSimpleUpdate msu;
	msu.setTable("user_info");
	msu.setUpdateValue("male", "0");
	msu.addUpdateValue("country", "99");
	msu.setCondition("mid", "test_update", true);
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msu));

	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.setTable("user_info");
	mss.setCondition("mid", "test_update", true);
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &mss));
	AST_EQ(1, mss.result.size());
	AST_EQ("99", mss.result[0][6]);
	

	MysqlSimpleDelete msd;
	msd.setTable("user_info");
	msd.setCondition("mid=\"test_update\"", false);
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msd));
    }
    

    void test_add(void) {
	MysqlSimpleSelect mss;
	mss.setField("*");
	mss.setTable("user_info");
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &mss));
	size_t total = mss.result.size();

	MysqlSimpleInsert msi;
	msi.setTable("user_info");
	msi.setField("mid");
	msi.addField("male");
	msi.setValue("test_add");
	msi.addValue("1");
	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &msi));

	AST_TRUE(MysqlConnPool::getInstance().doMysqlOperation((MysqlOperationBase *) &mss));
	size_t total2 = mss.result.size();
	AST_EQ(total + 1, total2);
    }

 private:
    bool _inited;
};
