#include "hallbase.h"
#include <util/stringutil.h>
#include <util/timeutil.h>

BEGIN_NAMESPACE(slots)

HallBase::HallBase():_client(cgserver::RedisClientFactory::getClient()){
}

HallBase::~HallBase(){
}

#define GENERATE_ROOM_KEY(hall, room, key)                              \
    std::string key = cgserver::StringUtil::toString(hall)              \
        + SlotCacheStr::sHallRoomJoiner + cgserver::StringUtil::toString(room);

#define PARSE_CACHE_VAL_TO_INT(bit, key, dest, dft)             \
    if (out[key].empty()) {                                     \
        dest = dft;                                             \
    } else if (!cgserver::StringUtil::StrToInt##bit(            \
                   out[key].data(), dest))                      \
    {                                                           \
        CLOG(WARNING) << "Invalid " << key << ":" << out[key];  \
        return false;                                           \
    }

#define EASY_REDIS_RESULT_CHECK(key, ret, ret_val)      \
    if (ret != 0) {                                     \
        CLOG(INFO) << "Get room info failed:" << key;   \
        return ret_val;                                 \
    }

#define EASY_REDIS_HSET(key, field, value)                              \
    {                                                                   \
        int ret = _client.Hset(key, field, value);                      \
        if (ret != RC_SUCCESS) {                                        \
            CLOG(WARNING) << "set [" << key << "." << field             \
                          << "] with ["<< value                         \
                          << "] failed. code:" << ret;                  \
        }                                                               \
    }

#define EASY_REDIS_HMSET(key, value)                                    \
    {                                                                   \
        int ret = _client.Hmset(key, value);                            \
        if (ret != RC_SUCCESS) {                                        \
            CLOG(WARNING) << "set [" << key                             \
                          << "] failed. code:" << ret;                  \
        }                                                               \
    }

bool HallBase::useRoom(int32_t hallID, int32_t userID, RoomInfo &targetRoom) {
    // get user info in redis
    int32_t curRoomID;
    if (!getCurrentRoomID(userID, hallID, curRoomID) || !getRoomInfo(
            hallID, targetRoom))
    {
        return false;
    }
    auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();

    bool ret = false;
    if (targetRoom.roomID == BLANK_ROOM_ID ||
        targetRoom.userID == userID ||
        targetRoom.userID == BLANK_USER_ID ||
        isRoomDead(targetRoom, now))
    {
        clearRoomUser(hallID, curRoomID);
        // update room last active time
        targetRoom.userID = userID;
        targetRoom.lastActive = now;
        updateRoomHBAndUser(hallID, targetRoom);
        //setRoomInfo(hallID, targetRoom);
        updateUserCurrentRoom(userID, hallID, targetRoom.roomID);
        ret = true;
    }
    return ret;
}

bool HallBase::getRoomInfo(int32_t hallID, RoomInfo &room) {
    std::map<std::string, std::string> out;
    GENERATE_ROOM_KEY(hallID, room.roomID, key);
    auto ret = _client.Hgetall(key, &out);
    if (ret != RC_SUCCESS) {
        room.userID = BLANK_USER_ID;
        return false;
    }
    PARSE_CACHE_VAL_TO_INT(32, SlotCacheStr::sRoomUserID, room.userID, BLANK_USER_ID);
    PARSE_CACHE_VAL_TO_INT(32, SlotCacheStr::sRoomGameCount, room.spinCount, 0);
    PARSE_CACHE_VAL_TO_INT(64, SlotCacheStr::sRoomPrize, room.totalPrize, 0);
    PARSE_CACHE_VAL_TO_INT(64, SlotCacheStr::sRoomLastHeartBeat, room.lastActive, 0);
    return true;
}

void HallBase::setRoomInfo(int32_t hallID, const RoomInfo &room) {
    GENERATE_ROOM_KEY(hallID, room.roomID, key);
    std::map<std::string, std::string> cacheInfo;
    cacheInfo[SlotCacheStr::sRoomUserID] = cgserver::StringUtil::toString(room.userID);
    cacheInfo[SlotCacheStr::sRoomGameCount] =
        cgserver::StringUtil::toString(room.spinCount);
    cacheInfo[SlotCacheStr::sRoomPrize] = cgserver::StringUtil::toString(room.totalPrize);
    cacheInfo[SlotCacheStr::sRoomLastHeartBeat] =
        cgserver::StringUtil::toString(room.lastActive);
    EASY_REDIS_HMSET(key, cacheInfo);
}

void HallBase::updateRoomResource(int32_t hallID, const RoomInfo &room) {
    GENERATE_ROOM_KEY(hallID, room.roomID, key);
    std::map<std::string, std::string> cacheInfo;
    cacheInfo[SlotCacheStr::sRoomGameCount] =
        cgserver::StringUtil::toString(room.spinCount);
    cacheInfo[SlotCacheStr::sRoomPrize] = cgserver::StringUtil::toString(room.totalPrize);
    EASY_REDIS_HMSET(key, cacheInfo);
}

bool HallBase::getCurrentRoomID(int32_t userID, int32_t hallID, int32_t &roomID) {
    std::string key = cgserver::StringUtil::toString(userID);
    std::string roomIDStr;
    auto ret = _client.Hget(key, SlotCacheStr::sUserCurRoom, &roomIDStr);
    EASY_REDIS_RESULT_CHECK(key, ret, false);
    if (roomIDStr.empty()) {
        roomID = BLANK_ROOM_ID;
        return true;
    }
    if (!cgserver::StringUtil::StrToInt32(roomIDStr.data(), roomID)) {
        CLOG(INFO) << "Parse room id failed:" << roomIDStr;
        return false;
    }
    roomID = roomID - HALL_FACTOR * hallID;
    if (roomID < 0 || roomID >= HALL_FACTOR) {
        roomID = BLANK_ROOM_ID;
    }
    return true;
}

void HallBase::updateRoomHBAndUser(int32_t hallID, const RoomInfo &room) {
    GENERATE_ROOM_KEY(hallID, room.roomID, key);
    std::map<std::string, std::string> cacheInfo;
    cacheInfo[SlotCacheStr::sRoomUserID] = cgserver::StringUtil::toString(room.userID);
    cacheInfo[SlotCacheStr::sRoomLastHeartBeat] =
        cgserver::StringUtil::toString(room.lastActive);
    EASY_REDIS_HMSET(key, cacheInfo);
}

void HallBase::updateUserCurrentRoom(int32_t userID, int32_t hallID, int32_t roomID) {
    int32_t realID = hallID * HALL_FACTOR + roomID;
    std::string key = cgserver::StringUtil::toString(userID);
    std::string val = cgserver::StringUtil::toString(realID);
    EASY_REDIS_HSET(key, SlotCacheStr::sUserCurRoom, val);
}

void HallBase::clearRoomUser(int32_t hallID, int32_t roomID) {
    if (roomID == BLANK_ROOM_ID) {
        return;
    }
    GENERATE_ROOM_KEY(hallID, roomID, key);
    EASY_REDIS_HSET(key, SlotCacheStr::sRoomUserID, SlotCacheStr::sBlankUserID);
}

bool HallBase::isRoomDead(const RoomInfo &room, int64_t now) {
    return now - room.lastActive > ROOM_RESERVE_TIME;
}

#undef EASY_REDIS_HSET
#undef EASY_REDIS_HMSET
#undef EASY_REDIS_RESULT_CHECK
#undef PARSE_CACHE_VAL_TO_INT
#undef GENERATE_ROOM_KEY
END_NAMESPACE
