#ifndef TSHALL_H
#define TSHALL_H

#include <util/common_define.h>
#include <slots/data/basicdata.h>

BEGIN_NAMESPACE(slots)

enum RoomStats {
  ERS_ROOM_FREE = 0,
  ERS_ROOM_BUSY = 1,
  ERS_ROOM_RESERVED = 2,
  ERS_ROOM_NOT_EXIST = 3,
  ERS_ROOM_UNKNOWN = 4
};

#define BLANK_USER_ID 0
#define BLANK_ROOM_ID 0

struct TSRoom {
  int32_t roomID{0};
  int32_t userID{0};
  int64_t lastActive{0};

  int64_t totalPrize{0};
  int32_t spinCount{0};
  int32_t totalLotteryCount{0};
  int32_t dayLotteryCount{0};
  int64_t totalOut{0};
  int64_t dayOut{0};

  RoomStats status{ERS_ROOM_FREE};
};
DF_SHARED_PTR(TSRoom);
typedef std::map<int32_t, TSRoomPtr> TSRooms;

class TSHall{
public:
  TSHall();
  ~TSHall();

  bool init(int32_t hallPrize, int32_t roomPrize, int32_t rTime);

  bool useRoom(int32_t userID, int32_t roomID);

  const TSRooms &getRooms();
  int64_t getRoomPrize(int32_t roomID);

  bool takeRoomTotalPrize(int32_t roomID, int64_t refill, int64_t &totalPrize);
  int64_t takeRoomPrize(int32_t roomID);
  int64_t takeHallPrize();

  bool reserveRoom(int32_t userID, int32_t roomID);

  bool incrRoomTotalPrize(int32_t roomID, int64_t totalPrize);
  void incrGameCount(int32_t roomID);
  void incrGameCount();
  void incrPrize(int64_t bet);/*add prize to hall*/
  void incrPrize(int32_t roomID, int64_t bet);/*add prize to room*/

  int32_t getGameCount(int32_t roomID);
  int32_t getGameCount();
  void updateRoomStatus();

private:
  bool getRoomByUserId(int32_t userID, TSRoomPtr &pRoom);
  bool getRoom(int32_t roomID, TSRoomPtr &room);
  bool createRoom(int32_t roomID, int64_t refill, TSRoomPtr &pRoom);
  bool createRoom(int32_t roomID, int64_t refill);

  bool leavingRoom(int32_t userID, TSRoomPtr pRoom);
  bool leavingRoom(int32_t userID, int32_t roomID);
  void freeRoom(TSRoom &room);

  std::mutex _lock;
  TSRooms _rooms;
  std::map<int32_t, int32_t> _user2room;

  int64_t spinCount{0};
  int64_t totalPrize{0};
  int32_t minRoomPrize{0};
  int32_t minHallPrize{0};
  int32_t reserveTime{600};
};

END_NAMESPACE
#endif
