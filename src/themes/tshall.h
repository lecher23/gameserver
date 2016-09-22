#ifndef TSHALL_H
#define TSHALL_H

#include <util/common_define.h>
#include <slots/data/basicdata.h>
#include <themes/tsconfig.h>

BEGIN_NAMESPACE(slots)

enum RoomStats {
  ERS_ROOM_FREE,
  ERS_ROOM_BUSY,
  ERS_ROOM_RESERVED,
  ERS_ROOM_NOT_EXIST,
  ERS_ROOM_UNKNOWN,
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
typedef std::vector<TSRoomPtr> TSRooms;

class TSHall{
public:
  TSHall();
  ~TSHall();

  /* Read config, prepare to use.*/
  bool init();

  bool process(GameResultData &data);

private:
  /*add prize to big prize pool*/
  void incrPrize(int64_t bet);
  /*add prize to specified machine*/
  void incrPrize(int32_t roomID, int64_t bet);
  bool getRoomByUserId(int32_t userID, TSRoomPtr &pRoom);
  bool getRoom(int32_t roomID, TSRoomPtr &room);
  bool getRooms(TSRooms &out);
  bool createRoom(int32_t roomID, int64_t refill, TSRoomPtr &pRoom);
  bool createRoom(int32_t roomID, int64_t refill);

  bool incrRoomTotalPrize(int32_t roomID, int64_t totalPrize);
  bool takeRoomTotalPrize(int32_t roomID, int64_t refill, int64_t &totalPrize);
  bool useRoom(int32_t userID, int32_t roomID);
  bool reserveRoom(int32_t userID, int32_t roomID);
  bool leavingRoom(int32_t userID, TSRoomPtr pRoom);
  bool leavingRoom(int32_t userID, int32_t roomID);

  std::mutex _lock;
  std::map<int32_t, TSRoomPtr> _rooms;
  std::map<int32_t, int32_t> _user2room;
  TSConfig _cfg;

  int64_t spinCount{0};
  int64_t poolSize{0};
};

END_NAMESPACE
#endif
