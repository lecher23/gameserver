#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>

#define private public
#include "slots/gameservice.h"
#undef private

#include "slots/slotsdatacenter.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y)
#define AST_TRUE(x) TS_ASSERT(x)

using namespace cgserver;
using namespace slots;
class TestGameService : public CxxTest::TestSuite 
{
 public:

 TestGameService():_inited(false){}

    virtual void setUp(){
	if (_inited) {
	    return;
	}
	_inited = true;
	// init db
	bool ret = MysqlClient::getInstance().initClient("127.0.0.1", "root", "111222", "SLOTS");
	AST_TRUE(ret);
	// init cache.
	SlotsDataCenter::instance().init(false);
	// init lua factory
	ret = LuaToolFactory::getInstance().init(2, "/home/licheng/workspace/lua/main.lua");
	AST_TRUE(ret);	
    }

    virtual void tearDown() {}
    
    void test_doSlots1( void )
    {

	GameService gs;
	CPacket p;
	SBuf bf;
	ResultFormatter rf;

	SlotsUserPtr spSlotsUser;
	std::string uid = "10";
	AST_TRUE(SlotsDataCenter::instance().get(uid, spSlotsUser));

	auto expect = spSlotsUser->uRes.fortune;
	
	p.addParam("uid", uid);
	p.addParam("type", "0");
	p.addParam("bet", "230");
	bool ret = gs.doSlots1(p, bf, rf);
	AST_TRUE(ret);

	// bet cost 230, and user earned 230. equal
	AST_EQ(expect, spSlotsUser->uRes.fortune);
    }

    void test_doMultiple_nodouble( void )
    {

	GameService gs;
	CPacket p;
	SBuf bf;
	ResultFormatter rf;

	SlotsUserPtr spSlotsUser;
	std::string uid = "10";
	AST_TRUE(SlotsDataCenter::instance().get(uid, spSlotsUser));

	auto expect = spSlotsUser->uRes.fortune;
	
	p.addParam("uid", uid);
	p.addParam("incr", "20");
	p.addParam("multiple", "0");
	bool ret = gs.doMultiple(p);
	AST_TRUE(ret);

	AST_EQ(expect - 20, spSlotsUser->uRes.fortune);
    }

    void test_doMultiple_double( void )
    {

	GameService gs;
	CPacket p;
	SBuf bf;
	ResultFormatter rf;

	SlotsUserPtr spSlotsUser;
	std::string uid = "10";
	AST_TRUE(SlotsDataCenter::instance().get(uid, spSlotsUser));

	auto expect = spSlotsUser->uRes.fortune;
	
	p.addParam("uid", uid);
	p.addParam("incr", "20");
	p.addParam("multiple", "2");
	bool ret = gs.doMultiple(p);
	AST_TRUE(ret);

	AST_EQ(expect + 20, spSlotsUser->uRes.fortune);
    }

    
    void test_doMultiple_negtive_incr( void )
    {

	GameService gs;
	CPacket p;
	SBuf bf;
	ResultFormatter rf;

	SlotsUserPtr spSlotsUser;
	std::string uid = "10";
	AST_TRUE(SlotsDataCenter::instance().get(uid, spSlotsUser));

	auto expect = spSlotsUser->uRes.fortune;
	
	p.addParam("uid", uid);
	p.addParam("incr", "-20");
	p.addParam("bet", "200");
	p.addParam("multiple", "0");
	bool ret = gs.doMultiple(p);
	AST_TRUE(!ret);

	AST_EQ(expect, spSlotsUser->uRes.fortune);
    }

    void test_doMultiple_invalid_multiple( void )
    {

	GameService gs;
	CPacket p;
	SBuf bf;
	ResultFormatter rf;

	SlotsUserPtr spSlotsUser;
	std::string uid = "10";
	AST_TRUE(SlotsDataCenter::instance().get(uid, spSlotsUser));

	auto expect = spSlotsUser->uRes.fortune;
	
	p.addParam("uid", uid);
	p.addParam("incr", "-20");
	p.addParam("bet", "200");
	p.addParam("multiple", "3");
	bool ret = gs.doMultiple(p);
	AST_TRUE(!ret);

	AST_EQ(expect, spSlotsUser->uRes.fortune);
    }

    
    void test_doSlots1_user_not_exist( void )
    {

	GameService gs;
	CPacket p;
	SBuf bf;
	ResultFormatter rf;

	std::string uid = "0";
	p.addParam("uid", uid);
	p.addParam("type", "0");
	p.addParam("bet", "230");
	bool ret = gs.doSlots1(p, bf, rf);
	AST_TRUE(!ret);
    }

    void test_doSlots1_negtive_bet( void )
    {

	GameService gs;
	CPacket p;
	SBuf bf;
	ResultFormatter rf;

	std::string uid = "0";
	p.addParam("uid", uid);
	p.addParam("type", "0");
	p.addParam("bet", "-230");
	bool ret = gs.doSlots1(p, bf, rf);
	AST_TRUE(!ret);
    }
    

 private:
    bool _inited;

};
