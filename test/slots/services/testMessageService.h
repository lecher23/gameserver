#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/data/gameresult.h>
#include <slots/services/messageservice.h>
#undef private
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testMessageService: public CxxTest::TestSuite 
{
public:
    testMessageService():_inited(false){}

    virtual void setUp(){
        CommonTools::init_slots_data_center_data();
    }

    virtual void tearDown() {
    }

    void test_finishTinyGame_success()
    {
        auto user = std::make_shared<SlotsUser>();
        user->uRes.incrFortune(100);
        ast_eq(100, user->uRes.fortune);
        auto &rh = user->gSt.getGameResult(1, 99);
        rh.earned.tinyGame = 300;
        rh.tinyGame.enable = true;
        ast_eq(300, user->gSt.tinyGameEarned());
        CommonTools::add_slots_user("21", user);

        CPacket packet;
        packet.addParam(slotconstants::sType, "5");
        packet.addParam(slotconstants::sUserID, "21");
        std::string out;
        ast_true(packet.getParamValue(slotconstants::sUserID, out));
        ast_eq("21", out);

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        ast_true(ms.finishTinyGame(packet, rf));
        ast_eq(400, user->uRes.fortune);
        ast_eq("{\"st\":\"OK\",\"f\":400}", bf.GetString());
    }

    void test_finishTinyGame_failed() {
        CPacket packet;
        packet.addParam(slotconstants::sType, "5");
        packet.addParam(slotconstants::sUserID, "21");
        std::string out;
        ast_true(packet.getParamValue(slotconstants::sUserID, out));
        ast_eq("21", out);

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        ast_true(!ms.finishTinyGame(packet, rf));
    }

    void test_finishTinyGame_notiny_game() {
        auto user = std::make_shared<SlotsUser>();
        user->uRes.incrFortune(100);
        ast_eq(100, user->uRes.fortune);
        auto &rh = user->gSt.getGameResult(1, 99);
        rh.earned.tinyGame = 300;
        rh.tinyGame.enable = false;
        ast_eq(0, user->gSt.tinyGameEarned());
        CommonTools::add_slots_user("21", user);

        CPacket packet;
        packet.addParam(slotconstants::sType, "5");
        packet.addParam(slotconstants::sUserID, "21");
        std::string out;
        ast_true(packet.getParamValue(slotconstants::sUserID, out));
        ast_eq("21", out);

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        ast_true(ms.finishTinyGame(packet, rf));
        ast_eq(100, user->uRes.fortune);
        ast_eq("{\"st\":\"OK\",\"f\":100}", bf.GetString());
    }

private:
    bool _inited;
};
