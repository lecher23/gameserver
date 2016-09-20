#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#define private public
#include <slots/roommanager.h>
#undef private
#include <util/timeutil.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testRoomManager: public CxxTest::TestSuite 
{
public:
testRoomManager():_inited(false){}

  virtual void setUp(){
  }

  virtual void tearDown() {
  }

  void test_getRoom_room_not_exist( void )
  {
    RoomManager rm;
    RoomInfoPtr pRoom;
    ast_true(!rm.getRoom(123, pRoom));
  }

  void test_getRoom_room_exist()
  {
    RoomManager rm;
    RoomInfoPtr pRoom;
    auto pRoom123 = std::make_shared<RoomInfo>();
    pRoom123->userID = 5;
    pRoom123->money = 99;
    rm._rooms[123] = pRoom123;
    ast_true(rm.getRoom(123, pRoom));
    ast_eq(5, pRoom->userID);
    ast_eq(99, pRoom->money);
  }

  void test_createRoom_room_not_exist(){
    RoomManager rm;
    ast_true(rm.createRoom(123, 99));
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(123, pRoom));
    ast_eq(ERS_ROOM_FREE, pRoom->status);
    ast_eq(99, pRoom->money);
    ast_eq(123, pRoom->roomID);
  }

  void test_createRoom_room_exist(){
    RoomManager rm;
    ast_true(rm.createRoom(123, 99));
    RoomInfoPtr pRoom;
    ast_true(!rm.createRoom(123, 199));
    ast_true(rm.getRoom(123, pRoom));
    ast_eq(99, pRoom->money);
  }

  void test_incrRoomMoney_room_not_exist() {
    RoomManager rm;
    ast_true(!rm.incrRoomMoney(123, 99));
  }

  void test_incrRoomMoney_room_exist() {
    RoomManager rm;
    ast_true(rm.createRoom(123, 100));
    ast_true(rm.incrRoomMoney(123, 99));
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(123, pRoom));
    ast_eq(199, pRoom->money);
  }

  void test_takeRoomMoney() {
    RoomManager rm;
    ast_true(rm.createRoom(123, 100));
    int64_t money;
    ast_true(rm.takeRoomMoney(123, 66, money));
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(123, pRoom));
    ast_eq(66, pRoom->money);
    ast_eq(100, money);
  }

  void test_useRoom_user_leaved_room_dest_room_not_exist() {
    RoomManager rm;
    // user 111 in room 222
    rm._user2room[111] = 0;
    // user 111 want use room 223
    ast_true(rm.useRoom(111, 223));
    // check
    ast_eq(223, rm._user2room[111]);

    // make sure room 223 exist
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(223, pRoom));
    ast_eq(ERS_ROOM_BUSY, pRoom->status);
    ast_eq(111, pRoom->userID);
  }

  void test_useRoom_user_not_in_room_dest_room_not_exist() {
    RoomManager rm;
    // user 111 want use room 223
    ast_true(rm.useRoom(111, 223));
    // check
    ast_eq(223, rm._user2room[111]);

    // make sure room 223 exist
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(223, pRoom));
    ast_eq(ERS_ROOM_BUSY, pRoom->status);
    ast_eq(111, pRoom->userID);
  }


  void test_useRoom_user_in_another_room_dest_room_not_exist() {
    RoomManager rm;
    RoomInfoPtr pRoomA;
    ast_true(rm.createRoom(222, 1000, pRoomA));
    // user 111 in room 222
    rm._user2room[111] = 222;
    pRoomA->status = ERS_ROOM_BUSY;
    pRoomA->userID = 111;
    // user 111 want use room 223
    ast_true(rm.useRoom(111, 223));
    // check
    ast_eq(ERS_ROOM_FREE, pRoomA->status);
    ast_eq(0, pRoomA->userID);
    ast_eq(223, rm._user2room[111]);

    // make sure room 223 exist
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(223, pRoom));
    ast_eq(ERS_ROOM_BUSY, pRoom->status);
    ast_eq(111, pRoom->userID);
  }

  void test_useRoom_user_in_another_room_dest_room_not_in_use() {
    RoomManager rm;
    RoomInfoPtr pRoomA;
    ast_true(rm.createRoom(222, 1000, pRoomA));
    // user 111 in room 222
    rm._user2room[111] = 222;
    pRoomA->status = ERS_ROOM_BUSY;
    pRoomA->userID = 111;
    // dest room not in use
    RoomInfoPtr pRoomB;
    ast_true(rm.createRoom(223, 2000, pRoomB));
    ast_eq(ERS_ROOM_FREE, pRoomB->status);
    // user 111 want use room 223
    ast_true(rm.useRoom(111, 223));
    // check
    ast_eq(ERS_ROOM_FREE, pRoomA->status);
    ast_eq(0, pRoomA->userID);
    ast_eq(223, rm._user2room[111]);

    // make sure room 223 exist
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(223, pRoom));
    ast_eq(ERS_ROOM_BUSY, pRoom->status);
    ast_eq(111, pRoom->userID);
  }

  void test_useRoom_user_in_another_room_dest_room_in_use_not_dead() {
    RoomManager rm;
    RoomInfoPtr pRoomA;
    ast_true(rm.createRoom(222, 1000, pRoomA));
    // dest room is used by user 112, and it is not dead
    RoomInfoPtr pRoomB;
    ast_true(rm.createRoom(223, 2000, pRoomB));
    pRoomB->status = ERS_ROOM_RESERVED;
    pRoomB->userID = 112;
    pRoomB->lastActive = cgserver::CTimeUtil::getCurrentTimeInSeconds();
    rm._user2room[112] = 223;
    // user 111 in room 222
    rm._user2room[111] = 222;
    pRoomA->status = ERS_ROOM_BUSY;
    pRoomA->userID = 111;
    // user 111 want use room 223
    ast_true(!rm.useRoom(111, 223));
    // check room 222 status
    ast_eq(ERS_ROOM_FREE, pRoomA->status);
    ast_eq(0, pRoomA->userID);
    // user leave room 222
    ast_eq(0, rm._user2room[111]);
    // check room 223 status
    ast_eq(ERS_ROOM_RESERVED, pRoomB->status);
    ast_eq(112, pRoomB->userID);
    ast_eq(223, rm._user2room[112]);
  }

  void test_useRoom_user_in_another_room_dest_room_in_use_dead() {
    RoomManager rm;
    RoomInfoPtr pRoomA;
    ast_true(rm.createRoom(222, 1000, pRoomA));
    // dest room is used by user 112, and it is not dead
    RoomInfoPtr pRoomB;
    ast_true(rm.createRoom(223, 2000, pRoomB));
    pRoomB->status = ERS_ROOM_RESERVED;
    pRoomB->userID = 112;
    pRoomB->lastActive = cgserver::CTimeUtil::getCurrentTimeInSeconds() - 800;
    rm._user2room[112] = 223;
    // user 111 in room 222
    rm._user2room[111] = 222;
    pRoomA->status = ERS_ROOM_BUSY;
    pRoomA->userID = 111;
    // user 111 want use room 223
    ast_true(rm.useRoom(111, 223));
    // check room 222 status
    ast_eq(ERS_ROOM_FREE, pRoomA->status);
    ast_eq(0, pRoomA->userID);
    // user in room 223
    ast_eq(223, rm._user2room[111]);
    // check room 223 status
    ast_eq(ERS_ROOM_BUSY, pRoomB->status);
    ast_eq(111, pRoomB->userID);
    // user 112 leaving room 223
    ast_eq(0, rm._user2room[112]);
    ast_true(cgserver::CTimeUtil::getCurrentTimeInSeconds() - pRoomB->lastActive < 2);
  }

  void test_reserveRoom() {
    RoomManager rm;
    ast_true(rm.createRoom(123, 100));
    RoomInfoPtr pRoom;
    ast_true(rm.getRoom(123, pRoom));
    ast_eq(ERS_ROOM_FREE, pRoom->status);
    ast_true(rm.useRoom(345, 123));
    ast_eq(ERS_ROOM_BUSY, pRoom->status);
    ast_eq(345, pRoom->userID);
    // invalid user
    ast_true(!rm.reserveRoom(344, 123));
    // valid user
    ast_true(rm.reserveRoom(345, 123));
    ast_eq(ERS_ROOM_RESERVED, pRoom->status);
  }

  void test_leavingRoom() {
    RoomManager rm;
    RoomInfoPtr pRoom;
    ast_true(rm.createRoom(123, 100, pRoom));
    // user 111 in room 123
    rm._user2room[111] = 123;
    pRoom->userID = 111;
    // invalid user
    ast_true(!rm.leavingRoom(344, 123));
    // valid user
    ast_true(rm.leavingRoom(111, 123));
    ast_eq(ERS_ROOM_FREE, pRoom->status);
    ast_eq(BLANK_USER_ID, pRoom->userID);
    ast_eq(0, rm._user2room[111]);
  }
private:
  bool _inited;
};
