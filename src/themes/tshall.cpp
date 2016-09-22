#include "tshall.h"

#include <util/timeutil.h>

BEGIN_NAMESPACE(slots)

TSHall::TSHall(){
}

TSHall::~TSHall(){
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

bool TSHall::getRooms(TSRooms &out) {
    for(auto &item: _rooms) {
        out.push_back(item.second);
    }
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

bool TSHall::incrRoomTotalPrize(int32_t roomID, int64_t totalPrize) {
    TSRoomPtr pRoom;
    if (!getRoom(roomID, pRoom)) {
        return false;
    }
    pRoom->totalPrize += totalPrize;
    return true;
}

bool TSHall::takeRoomTotalPrize(int32_t roomID, int64_t refill, int64_t &totalPrize) {
    TSRoomPtr pRoom;
    if(!getRoom(roomID, pRoom)) {
        return false;
    }
    totalPrize = pRoom->totalPrize;
    pRoom->totalPrize = refill;
    return true;
}

bool TSHall::useRoom(int32_t userID, int32_t roomID) {
    bool ret = false;
    do {
        auto itr = _user2room.find(userID);
        if(itr != _user2room.end() && itr->second != 0) {
            CLOG(INFO) << "leaving room " << itr->second;
            // leaving old room
            leavingRoom(userID, itr->second);
        }
        TSRoomPtr pRoom;
        auto now = cgserver::CTimeUtil::getCurrentTimeInSeconds();
        if(!getRoom(roomID, pRoom)) {
            CLOG(INFO) << "Room "<< roomID <<" not exist ";
            // room not exist, create it
            ret = createRoom(roomID, _cfg.rMinPoolSize, pRoom);
        } else if (pRoom->status != ERS_ROOM_FREE){
            CLOG(INFO) << "Room "<< roomID <<" not free ";
            if (now - pRoom->lastActive < _cfg.rRoomReserveTime) {
                CLOG(INFO) << "Room "<< roomID <<" not expire. ";
                // room in using
                break;
            }
            CLOG(INFO) << "force user " << pRoom->userID << " leaving room " << pRoom->roomID;
            // force dead user leaving
            leavingRoom(pRoom->userID, pRoom);
            pRoom->lastActive = now;
        }
        CLOG(INFO) << "user " << userID << " take room " << pRoom->roomID;
        // take this room
        pRoom->status = ERS_ROOM_BUSY;
        pRoom->userID = userID;
        _user2room[userID] = roomID;
        pRoom->lastActive = now;
        ret = true;
    }while (false);
    return ret;
}

bool TSHall::reserveRoom(int32_t userID, int32_t roomID) {
    TSRoomPtr pRoom;
    if(!getRoom(roomID, pRoom) || pRoom->userID != userID) {
        return false;
    }
    pRoom->status = ERS_ROOM_RESERVED;
    return true;
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

END_NAMESPACE
