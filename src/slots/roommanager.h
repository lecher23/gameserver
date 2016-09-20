#ifndef ROOMMANAGER_H
#define ROOMMANAGER_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

enum RoomStats {
  ERS_ROOM_FREE,
  ERS_ROOM_BUSY,
  ERS_ROOM_RESERVED,
  ERS_ROOM_NOT_EXIST,
  ERS_ROOM_UNKNOWN,
};

#define BLANK_USER_ID 0

struct RoomInfo{
  int32_t roomID;
  int32_t userID;
  int64_t money;
  RoomStats status;
 RoomInfo():userID(BLANK_USER_ID), money(0), status(ERS_ROOM_FREE) {}
};
DF_SHARED_PTR(RoomInfo);
typedef std::vector<RoomInfoPtr> RoomsInfo;

class RoomManager{
public:
  RoomManager();
  ~RoomManager();

  bool getRoom(int32_t roomID, RoomInfoPtr &room);
  bool getRooms(RoomsInfo &out);
  bool createRoom(int32_t roomID, int64_t refill);

  bool incrRoomMoney(int32_t roomID, int64_t money);
  bool takeRoomMoney(int32_t roomID, int64_t refill, int64_t &money);
  bool useRoom(int32_t userID, int32_t roomID);
  bool reserveRoom(int32_t userID, int32_t roomID);
  bool leavingRoom(int32_t userID, int32_t roomID);

private:
  std::map<int32_t, RoomInfoPtr> _rooms;
};

END_NAMESPACE
#endif
