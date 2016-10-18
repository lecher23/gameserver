#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/services/updateservice.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testUpdateService: public CxxTest::TestSuite 
{
public:
    testUpdateService():_inited(false){}

    virtual void setUp(){
        CommonTools::init_slots_data_center_data();
    }

    virtual void tearDown() {
    }

    void test_update_user_info_no_blank( void )
    {
        auto user = std::make_shared<SlotsUser>();
        user->uRes.incrFortune(100);
        user->uInfo.male = "1";
        user->uInfo.fname = "test1";
        user->uInfo.country = "34";
        user->uInfo.avatar = "1.jpg";
        ast_eq(100, user->uRes.fortune);
        auto &rh = user->gSt.getGameResult(1, 99);
        rh.earned.tinyGame = 300;
        rh.tinyGame.enable = true;
        ast_eq(300, user->gSt.tinyGameEarned());
        CommonTools::add_slots_user("21", user);

        CPacket packet;
        packet.addParam(slotconstants::sType, "1");
        packet.addParam(slotconstants::sUserID, "21");
        packet.addParam(slotconstants::sMale, "0");
        packet.addParam(slotconstants::sName, "test2");
        packet.addParam(slotconstants::sCountry, "86");
        packet.addParam(slotconstants::sAvatar, "2.png");

        UpdateService us;
        CResponse resp;
        ast_true(us.doJob(packet, resp));
        ast_eq("0", user->uInfo.male);
        ast_eq("test2", user->uInfo.fname);
        ast_eq("86", user->uInfo.country);
        ast_eq("2.png", user->uInfo.avatar);
    }

    void test_update_user_info_with_blank() {
        auto user = std::make_shared<SlotsUser>();
        user->uRes.incrFortune(100);
        user->uInfo.male = "1";
        user->uInfo.fname = "test1";
        user->uInfo.country = "34";
        user->uInfo.avatar = "1.jpg";
        ast_eq(100, user->uRes.fortune);
        auto &rh = user->gSt.getGameResult(1, 99);
        rh.earned.tinyGame = 300;
        rh.tinyGame.enable = true;
        ast_eq(300, user->gSt.tinyGameEarned());
        CommonTools::add_slots_user("21", user);

        CPacket packet;
        packet.addParam(slotconstants::sType, "1");
        packet.addParam(slotconstants::sUserID, "21");
        packet.addParam(slotconstants::sMale, "0");
        packet.addParam(slotconstants::sName, "");

        UpdateService us;
        CResponse resp;
        ast_true(us.doJob(packet, resp));
        ast_eq("0", user->uInfo.male);
        ast_eq("test1", user->uInfo.fname);
        ast_eq("34", user->uInfo.country);
        ast_eq("1.jpg", user->uInfo.avatar);
    }
private:
    bool _inited;
};
