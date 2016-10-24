#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#define private public
#include <slots/data/gameresult.h>
#include <slots/services/messageservice.h>
#undef private
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#include <util/stringutil.h>
#include <util/timeutil.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testMessageService: public CxxTest::TestSuite 
{
public:
    testMessageService():_inited(false){
    }

    virtual void setUp(){
        CommonTools::init_slots_data_center_data();
        if (!_inited) {
            ast_true(RedisClientFactory::getClient().Initialize(
                         "127.0.0.1", 6379, 2, 3));
            ast_true(SlotsConfig::getInstance().
                     onlineConfig.initFromJsonFile(
                             "config_files/OnlineConfigTest.json"));
            _inited = true;
        }
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

    void test_getRoomInfoInList() {
        CPacket packet;
        packet.addParam(slotconstants::sType, "6");
        packet.addParam(slotconstants::sHallID, "1");
        packet.addParam(slotconstants::sRoomList, "2,3");
        auto &client = RedisClientFactory::getClient();
        auto now = CTimeUtil::getCurrentTimeInSeconds();
        ast_eq(0, client.Hset("1.2", "prz", "1000"));
        ast_eq(0, client.Hset("1.2", "uid", "35"));
        ast_eq(0, client.Hset("1.2", "lhb", StringUtil::toString(now)));

        auto pre = now - 10000;
        ast_eq(0, client.Hset("1.3", "prz", "3219"));
        ast_eq(0, client.Hset("1.3", "uid", "23"));
        ast_eq(0, client.Hset("1.3", "lhb", StringUtil::toString(pre)));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        bool ret = ms.getRoomInfoInList(packet, rf);
        ast_true(ret);
        ast_eq("{\"rl\":[{\"id\":2,\"pz\":1000,\"u\":35},{\"id\":3,\"pz\":3219,\"u\":0}],\"st\":\"OK\"}",
               bf.GetString());
    }

    void test_getLoginReward(){
        CPacket packet;
        packet.addParam(slotconstants::sType, "1");
        packet.addParam(slotconstants::sUserID, "12");
        SlotsUserData sud;
        LoginReward lr;
        lr.recved = false;
        lr.runnerReward = 300;
        lr.dayReward = 400;
        lr.vipExtra = 100;
        sud.setDailyReward("12", lr);
        SlotsUserPtr user(new SlotsUser);
        user->uRes.fortune = 80;
        CommonTools::add_slots_user("12", user);

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        ast_true(ms.getLoginReward(packet, out));
        ast_eq(880, out);
        ast_true(!lr.recved);
    }

    void test_getHallInfoInList_hall_not_exist(){
        CPacket packet;
        packet.addParam(slotconstants::sType, "10");
        packet.addParam(slotconstants::sHallList, "76, 98");
        auto &client = RedisClientFactory::getClient();
        ast_eq(0, client.Hset("H:prz", "76", "1000"));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        ast_true(ms.getHallInfoInList(packet, rf));
        ast_eq("{\"rl\":[{\"id\":\"76\",\"pz\":\"1000\"},{\"id\":\" 98\",\"pz\":\"\"}],\"st\":\"OK\"}",
               bf.GetString());
        ast_eq(0, client.Hdel("H:prz", "76"));
    }

    void test_getHallInfoInList_with_abnormal_list(){
        CPacket packet;
        packet.addParam(slotconstants::sType, "10");
        packet.addParam(slotconstants::sHallList, "76, 98, ,,");
        auto &client = RedisClientFactory::getClient();
        ast_eq(0, client.Hset("H:prz", "76", "1000"));
        ast_eq(0, client.Hset("H:prz", "98", "399"));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        ast_true(ms.getHallInfoInList(packet, rf));
        ast_eq("{\"rl\":[{\"id\":\"76\",\"pz\":\"1000\"},{\"id\":\" 98\",\"pz\":\"\"}],\"st\":\"OK\"}",
               bf.GetString());
        ast_eq(0, client.Hdel("H:prz", "76"));
        ast_eq(0, client.Hdel("H:prz", "98"));
    }
    void test_getHallInfoInList(){
        CPacket packet;
        packet.addParam(slotconstants::sType, "10");
        packet.addParam(slotconstants::sHallList, "76,98");
        auto &client = RedisClientFactory::getClient();
        ast_eq(0, client.Hset("H:prz", "76", "1000"));
        ast_eq(0, client.Hset("H:prz", "98", "399"));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        ast_true(ms.getHallInfoInList(packet, rf));
        ast_eq("{\"rl\":[{\"id\":\"76\",\"pz\":\"1000\"},{\"id\":\"98\",\"pz\":\"399\"}],\"st\":\"OK\"}",
               bf.GetString());
        ast_eq(0, client.Hdel("H:prz", "76"));
        ast_eq(0, client.Hdel("H:prz", "98"));
    }

    void test_reportOnlineTime_not_recv_reward() {
        CPacket packet;
        packet.addParam(slotconstants::sType, "8");
        packet.addParam(slotconstants::sUserID, "23");
        packet.addParam(slotconstants::sOnlineTime, "10");
        auto &client = RedisClientFactory::getClient();
        ast_eq(0, client.Hset("L23", "ol:time", "100"));
        ast_eq(0, client.Hset("L23", "ol:level", "1"));
        ast_eq(0, client.Hset("L23", "ol:recv", "0"));
        ast_eq(0, client.Hset("L23", "ol:rwd", "500"));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        ast_true(ms.reportOnlineTime(packet, rf));
        ast_eq("{\"st\":\"OK\",\"cl\":1,\"tl\":1,\"r\":false,\"v\":500,\"tr\":86400}",
               bf.GetString());
        std::string tmp;
        ast_eq(0, client.Hget("L23", "ol:level", &tmp));
        ast_eq("1", tmp);
        ast_eq(0, client.Hget("L23", "ol:time", &tmp));
        ast_eq("100", tmp);
        ast_eq(0, client.Hget("L23", "ol:rwd", &tmp));
        ast_eq("500", tmp);
        ast_eq(0, client.Hget("L23", "ol:recv", &tmp));
        ast_eq("0", tmp);
        ast_eq(0, client.Del("L23"));
    }

    void test_reportOnlineTime_recv_reward() {
        CPacket packet;
        packet.addParam(slotconstants::sType, "8");
        packet.addParam(slotconstants::sUserID, "23");
        packet.addParam(slotconstants::sOnlineTime, "10");
        auto &client = RedisClientFactory::getClient();
        ast_eq(0, client.Hset("L23", "ol:time", "1300"));
        ast_eq(0, client.Hset("L23", "ol:level", "1"));
        ast_eq(0, client.Hset("L23", "ol:recv", "1"));
        ast_eq(0, client.Hset("L23", "ol:rwd", "500"));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        ast_true(ms.reportOnlineTime(packet, rf));
        ast_eq("{\"st\":\"OK\",\"cl\":1,\"tl\":2,\"r\":false,\"v\":500,\"tr\":3600}",
               bf.GetString());
        std::string tmp;
        ast_eq(0, client.Hget("L23", "ol:level", &tmp));
        ast_eq("2", tmp);
        ast_eq(0, client.Hget("L23", "ol:time", &tmp));
        ast_eq("0", tmp);
        ast_eq(0, client.Hget("L23", "ol:rwd", &tmp));
        ast_eq("500", tmp);
        ast_eq(0, client.Del("L23"));
    }

    void test_reportOnlineTime_user_not_exist() {
        CPacket packet;
        packet.addParam(slotconstants::sType, "8");
        packet.addParam(slotconstants::sUserID, "23");
        packet.addParam(slotconstants::sOnlineTime, "10");
        auto &client = RedisClientFactory::getClient();
        ast_eq(0, client.Del("L23"));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        ast_true(ms.reportOnlineTime(packet, rf));
        ast_eq("{\"st\":\"OK\",\"cl\":0,\"tl\":0,\"r\":false,\"v\":500,\"tr\":86400}",
               bf.GetString());
        std::string tmp;
        ast_eq(0, client.Del("L23"));
    }

    void test_reportOnlineTime_loop() {
        CPacket packet;
        packet.addParam(slotconstants::sType, "8");
        packet.addParam(slotconstants::sUserID, "23");
        packet.addParam(slotconstants::sOnlineTime, "400");
        auto &client = RedisClientFactory::getClient();
        ast_eq(0, client.Hset("L23", "ol:time", "3200"));
        ast_eq(0, client.Hset("L23", "ol:level", "4"));
        ast_eq(0, client.Hset("L23", "ol:recv", "1"));
        ast_eq(0, client.Hset("L23", "ol:rwd", "500"));

        MessageService ms;
        SBuf bf;
        ResultFormatter rf(bf);
        int64_t out;
        std::string tmp;
        ast_true(ms.reportOnlineTime(packet, rf));
        ast_eq(0, client.Hget("L23", "ol:rwd", &tmp));
        ast_eq("{\"st\":\"OK\",\"cl\":4,\"tl\":0,\"r\":false,\"v\":" + tmp + ",\"tr\":0}",
               bf.GetString());
        ast_eq(0, client.Hget("L23", "ol:level", &tmp));
        ast_eq("0", tmp);
        ast_eq(0, client.Hget("L23", "ol:time", &tmp));
        ast_eq("0", tmp);
        ast_eq(0, client.Del("L23"));
    }

private:
    bool _inited;
};
