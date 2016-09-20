#include "roommanager.h"

BEGIN_NAMESPACE(slots)

RoomManager::RoomManager(){
}

RoomManager::~RoomManager(){
}

bool RoomManager::getRoom(int32_t roomID, RoomInfoPtr &pRoom) {
    auto itr = _rooms.find(roomID);
    if(itr == _rooms.end()) {
        return false;
    }
    pRoom = itr->second;
    return true;
}

bool RoomManager::getRooms(RoomsInfo &out) {
    for(auto &item: _rooms) {
        out.push_back(item.second);
    }
    return true;
}

bool RoomManager::createRoom(int32_t roomID, int64_t refill) {
    auto itr = _rooms.find(roomID);
    if(itr == _rooms.end()) {
        RoomInfoPtr pRoom = std::make_shared<RoomInfo>();
        pRoom->money = refill;
        pRoom->roomID = roomID;
        _rooms[roomID] = pRoom;
        return true;
    }
    return false;
}

bool RoomManager::incrRoomMoney(int32_t roomID, int64_t money) {
    RoomInfoPtr pRoom;
    if (!getRoom(roomID, pRoom)) {
        return false;
    }
    pRoom->money += money;
    return true;
}

bool RoomManager::takeRoomMoney(int32_t roomID, int64_t refill, int64_t &money) {
    RoomInfoPtr pRoom;
    if(!getRoom(roomID, pRoom)) {
        return false;
    }
    money = pRoom->money;
    pRoom->money = refill;
    return true;
}

bool RoomManager::useRoom(int32_t userID, int32_t roomID) {
    RoomInfoPtr pRoom;
    if(!getRoom(roomID, pRoom) || pRoom->status != ERS_ROOM_FREE) {
        return false;
    }
    pRoom->status = ERS_ROOM_BUSY;
    pRoom->userID = userID;
    return true;
}

bool RoomManager::reserveRoom(int32_t userID, int32_t roomID) {
    RoomInfoPtr pRoom;
    if(!getRoom(roomID, pRoom) || pRoom->userID != userID) {
        return false;
    }
    pRoom->status = ERS_ROOM_RESERVED;
    return true;
}

bool RoomManager::leavingRoom(int32_t userID, int32_t roomID) {
    RoomInfoPtr pRoom;
    if(!getRoom(roomID, pRoom) || pRoom->userID != userID) {
        return false;
    }
    pRoom->status = ERS_ROOM_FREE;
    pRoom->userID = BLANK_USER_ID;
    return true;
}

END_NAMESPACE
