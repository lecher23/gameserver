#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/themes/hallbase.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testHallBase: public CxxTest::TestSuite 
{
public:
    testHallBase():_inited(false){
        if (!_inited) {
            ast_true(RedisClientFactory::getClient().Initialize(
                         "127.0.0.1", 6379, 2, 3));
            _inited = true;
        }
    }

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_operateHallPrize( void )
    {
        auto &client = RedisClientFactory::getClient();
        ast_eq(RC_SUCCESS, client.Hset("H:prz", "13", "1000"));

        HallBase hb;
        ast_eq(1999, hb.incrHallPrize(13, 999));
        ast_eq(RC_SUCCESS, client.Hdel("H:prz", "13"));
    }

#define SET_ROOM_INFO_TO_REDIS(key, userID, lastHB)                     \
    ast_eq(RC_SUCCESS, client.Hset(key, SlotCacheStr::sRoomUserID, userID)); \
    ast_eq(RC_SUCCESS, client.Hset(                                     \
               key, SlotCacheStr::sRoomLastHeartBeat, StringUtil::toString(lastHB)));

    void test_useRoom_cur_in_anoter_hall_target_using() {
        // target hall:1, target room 1
        // cur hall:2, cur room 2
        auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
        auto &client = RedisClientFactory::getClient();
        // set crrent info
        ast_eq(RC_SUCCESS, client.Hset("10086", SlotCacheStr::sUserCurRoom, "2000002"));
        // set target room info
        SET_ROOM_INFO_TO_REDIS("1.1", "18", now);
        // set cur room info
        SET_ROOM_INFO_TO_REDIS("2.1", "23", now);

        HallBase hb;
        RoomInfo rm;
        rm.roomID = 1;
        ast_true(!hb.useRoom(1, 23, rm));
        ast_eq(RC_SUCCESS, client.Del("10086"));
    }

    void test_useRoom_cur_in_anoter_hall_target_expire() {
        // target hall:1, target room 1
        // cur hall:2, cur room 2
        auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
        auto &client = RedisClientFactory::getClient();
        // set crrent info: hall[2], room[2]
        ast_eq(RC_SUCCESS, client.Hset("10086", SlotCacheStr::sUserCurRoom, "2000002"));
        // set target room info: hall[1], room[1]
        auto pre = now - 10000;
        SET_ROOM_INFO_TO_REDIS("1.1", "18", pre);
        // set cur room info: hall[2], room[2], used by 23
        SET_ROOM_INFO_TO_REDIS("2.2", "23", now);

        HallBase hb;
        RoomInfo rm;
        rm.roomID = 1;
        auto ret = hb.useRoom(1, 23, rm);
        ast_true(ret);
        ast_eq(RC_SUCCESS, client.Del("10086"));
    }

    void test_incrHallPrize_hall_not_exist(){
        HallBase hb;
        auto &client = RedisClientFactory::getClient();
        ast_eq(RC_SUCCESS, client.Hdel("H:prz", "13"));
        ast_eq(100, hb.incrHallPrize(13, 100));
        ast_eq(RC_SUCCESS, client.Hdel("H:prz", "13"));
    }
private:
    bool _inited;
};
