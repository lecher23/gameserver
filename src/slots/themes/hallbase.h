#ifndef HALLBASE_H
#define HALLBASE_H

#include <util/common_define.h>
#include <cache/redisclientfactory.h>

BEGIN_NAMESPACE(slots)

namespace SlotCacheStr{
  const std::string sHallRoomJoiner = ".";
  const std::string sHallPrize = "H:prz";
  const std::string sHallGameCount = "H:gc";
  const std::string sRoomUserID = "uid";
  const std::string sRoomPrize = "prz";
  const std::string sRoomLastHeartBeat = "lhb";
  const std::string sRoomGameCount = "gc";
  const std::string sUserCurRoom = "room";
  const std::string sBlankUserID = "0";
};

#define BLANK_USER_ID 0
#define BLANK_ROOM_ID 0
#define HALL_FACTOR 1000000
#define ROOM_RESERVE_TIME 600

class RoomInfo {
public:
  void update(int64_t now, int64_t reserveTime) {
    if (userID != BLANK_ROOM_ID && now - lastActive > reserveTime)
      userID = BLANK_USER_ID;
  }
  void takePrize(int64_t remain) {totalPrize = remain;++winnigCount;}
  void incrSpin() {++spinCount;}
  void incrPrize(int64_t incr) {totalPrize += incr;}

  int32_t roomID{0};
  int32_t userID{0};
  int64_t lastActive{0};
  int64_t totalPrize{0};
  int32_t spinCount{0};
  int32_t winnigCount{0};
  /* int32_t dayLotteryCount{0}; */
  /* int64_t totalOut{0}; */
  /* int64_t dayOut{0}; */
};
DF_SHARED_PTR(RoomInfo);
typedef std::vector<RoomInfo> RoomInfos;

class HallBase{
 public:
    HallBase();
    virtual ~HallBase();

    bool useRoom(int32_t hallID, int32_t userID, RoomInfo &room);
    bool getRoomInfo(int32_t hallID, RoomInfo &room);
    void setRoomInfo(int32_t hallID, const RoomInfo &room);
    void updateRoomResource(int32_t hallID, const RoomInfo &room);
    int64_t incrHallPrize(int32_t hallID, int64_t val);
    int32_t incrHallGameCount(int32_t hallID, int32_t incr = 1);
    int64_t getHallPrize(int32_t hallID);
    int32_t getHallGameCount(int32_t hallID);

protected:
    bool getCurrentRoomID(int32_t userID, int32_t hallID, int32_t &roomID);
    void updateRoomHBAndUser(int32_t hallID, const RoomInfo &room);
    void updateUserCurrentRoom(int32_t userID, int32_t hallID, int32_t roomID);
    void clearRoomUser(int32_t hallID, int32_t roomID);
    bool isRoomDead(const RoomInfo &room, int64_t now);
    int32_t getRoomUser(int32_t hallID, int32_t roomID, int32_t userID);

    CRedisClient &_client;
};

END_NAMESPACE
#endif
