#include "messageservice.h"
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

MessageService::MessageService(){
}

MessageService::~MessageService(){
}

bool MessageService::doJob(CPacket &packet, CResponse &resp) {
    static const std::string sFortune = "fortune";
    int32_t gType;
    if (!getIntVal(packet, "type", gType)) {
        return false;
    }
    SBuf bf;
    ResultFormatter rf(bf);
    bool ret = false;
    switch(gType){
    case 1:{
        int64_t newFortune;
        ret = getLoginReward(packet, newFortune);
        rf.formatSimpleResult(ret, sFortune, newFortune);
        break;
    }
    default:
        break;
    }
    resp.setBody(bf.GetString());
    return ret;
}

bool MessageService::getLoginReward(CPacket &packet, int64_t &newFortune) {
    std::string uid;
    GET_PARAM("uid", uid, true);
    SlotsUserPtr user;
    if (!SlotsDataCenter::instance().slotsUserData->getByUid(uid, user)) {
        return false;
    }
    auto &loginReward = user->loginReward;
    if (loginReward.recved) {
        return true;
    }
    int64_t total = loginReward.runnerReward +
        loginReward.daysReward + loginReward.specialReward;
    user->uRes.incrFortune(total);
    loginReward.recved = true;
    newFortune = user->uRes.fortune;
    return true;
}

bool MessageService::getCargoStatus(CPacket &packet) {
    std::string uid;
    GET_PARAM("uid", uid, true);
    std::string cargoId;
    GET_PARAM("cargo", cargoId, true);
    // TODO
    // get cargo from cache
    // if no record, async request server
    // if has record, return value
    return false;
}

END_NAMESPACE
