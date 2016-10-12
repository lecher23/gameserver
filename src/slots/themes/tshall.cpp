#include "tshall.h"

#include <util/timeutil.h>

BEGIN_NAMESPACE(slots)

TSHall::TSHall(){
}

TSHall::~TSHall(){
}

bool TSHall::init(int32_t hallPrize, int32_t roomPrize, int32_t rTime) {
    minHallPrize = hallPrize;
    minRoomPrize = roomPrize;
    reserveTime = rTime;
    return true;
}

bool TSHall::useRoom(int32_t userID, int32_t roomID) {
    bool ret = false;
    do {
        auto itr = _user2room.find(userID);
        if(itr != _user2room.end() && itr->second != 0) {
            if (itr->second == roomID) {
                // user already in this room.
                return true;
            }
            //CLOG(INFO) << "leaving room " << itr->second;
            leavingRoom(userID, itr->second);
        }
        TSRoomPtr pRoom;
        auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
        if(!getRoom(roomID, pRoom)) {
            //CLOG(INFO) << "Room "<< roomID <<" not exist ";
            ret = createRoom(roomID, minRoomPrize, pRoom);
        } else if (pRoom->status != ERS_ROOM_FREE){
            //CLOG(INFO) << "Room "<< roomID <<" not free ";
            if (now - pRoom->lastActive < reserveTime) {
                CLOG(ERROR) << "Room "<< roomID <<" in use. ";
                // room in using
                break;
            }
            //CLOG(INFO) << "force user " << pRoom->userID << " leaving room " << pRoom->roomID;
            // force dead user leaving
            leavingRoom(pRoom->userID, pRoom);
            pRoom->lastActive = now;
        }
        // take this room
        pRoom->status = ERS_ROOM_BUSY;
        pRoom->userID = userID;
        _user2room[userID] = roomID;
        pRoom->lastActive = now;
        ret = true;
    }while (false);
    return ret;
}

const TSRooms &TSHall::getRooms() {
    updateRoomStatus();
    return _rooms;
}

int64_t TSHall::getRoomPrize(int32_t roomID) {
    TSRoomPtr ret;
    if (!getRoom(roomID, ret)) {
        return 0;
    }
    return ret->totalPrize;
}

int64_t TSHall::takeRoomPrize(int32_t roomID) {
    MUTEX_GUARD(_lock);
    TSRoomPtr pRoom;
    if(!getRoom(roomID, pRoom)) {
        return 0;
    }
    auto tmp = pRoom->totalPrize;
    pRoom->totalPrize = minRoomPrize;
    return tmp;
}

int64_t TSHall::takeHallPrize() {
    MUTEX_GUARD(_lock);
    auto tmp = totalPrize;
    totalPrize = minHallPrize;
    return tmp;
}

bool TSHall::reserveRoom(int32_t userID, int32_t roomID) {
    TSRoomPtr pRoom;
    if(!getRoom(roomID, pRoom) || pRoom->userID != userID) {
        return false;
    }
    pRoom->status = ERS_ROOM_RESERVED;
    return true;
}

void TSHall::incrGameCount(int32_t roomID) {
    MUTEX_GUARD(_lock);
    TSRoomPtr pRoom;
    if (getRoom(roomID, pRoom)) {
        ++pRoom->spinCount;
    }
}

void TSHall::incrGameCount() {
    MUTEX_GUARD(_lock);
    ++spinCount;
}

void TSHall::incrPrize(int64_t prize) {
    MUTEX_GUARD(_lock);
    totalPrize += prize;
}

void TSHall::incrPrize(int32_t roomID, int64_t totalPrize) {
    TSRoomPtr pRoom;
    if (getRoom(roomID, pRoom)) {
        pRoom->totalPrize += totalPrize;
    }
}

int32_t TSHall::getGameCount(int32_t roomID) {
    TSRoomPtr pRoom;
    if (!getRoom(roomID, pRoom)) {
        CLOG(WARNING) << "Invalid room id:" << roomID;
        return 0;
    }
    return pRoom->spinCount;
}

int32_t TSHall::getGameCount() {
    return spinCount;
}

void TSHall::updateRoomStatus() {
    auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
    for (auto &item: _rooms) {
        auto &room = *(item.second);
        if (room.userID == BLANK_ROOM_ID) {
            continue;
        }
        if (now - room.lastActive > reserveTime) {
            CLOG(ERROR) << "Room "<< room.roomID <<" out of date.";
            freeRoom(room);
        }
    }
}

bool TSHall::getRoomByUserId(int32_t userID, TSRoomPtr &pRoom) {
    auto itr = _user2room.find(userID);
    if (itr == _user2room.end() || itr->second == BLANK_ROOM_ID) {
        return false;
    }
    return getRoom(itr->second, pRoom);
}

bool TSHall::getRoom(int32_t roomID, TSRoomPtr &pRoom) {
    auto itr = _rooms.find(roomID);
    if(itr == _rooms.end()) {
        return false;
    }
    pRoom = itr->second;
    return true;
}

bool TSHall::createRoom(int32_t roomID, int64_t refill) {
    auto itr = _rooms.find(roomID);
    if(itr == _rooms.end()) {
        auto pRoom = std::make_shared<TSRoom>();
        pRoom->totalPrize = refill;
        pRoom->roomID = roomID;
        pRoom->lastActive = cgserver::CTimeUtil::getCurrentTimeInSeconds();
        _rooms[roomID] = pRoom;
        return true;
    }
    return false;
}

bool TSHall::createRoom(int32_t roomID, int64_t refill, TSRoomPtr &pRoom) {
    auto itr = _rooms.find(roomID);
    if(itr == _rooms.end()) {
        pRoom = std::make_shared<TSRoom>();
        pRoom->totalPrize = refill;
        pRoom->roomID = roomID;
        pRoom->lastActive = cgserver::CTimeUtil::getCurrentTimeInSeconds();
        _rooms[roomID] = pRoom;
        return true;
    }
    return false;
}

bool TSHall::leavingRoom(int32_t userID, TSRoomPtr pRoom) {
    _user2room[userID] = 0;
    if (pRoom.get() != nullptr) {
        pRoom->status = ERS_ROOM_FREE;
        pRoom->userID = BLANK_USER_ID;
    }
    return true;
}

bool TSHall::leavingRoom(int32_t userID, int32_t roomID) {
    TSRoomPtr pRoom;
    if(!getRoom(roomID, pRoom) || pRoom->userID != userID) {
        CLOG(WARNING) << "leaving wrong room: " << roomID;
        // leaving wrong room
        return false;
    }
    leavingRoom(userID, pRoom);
    return true;
}

void TSHall::freeRoom(TSRoom &pRoom) {
    auto userID = pRoom.userID;
    _user2room[userID] = 0;
    pRoom.status = ERS_ROOM_FREE;
    pRoom.userID = BLANK_USER_ID;
}

END_NAMESPACE
