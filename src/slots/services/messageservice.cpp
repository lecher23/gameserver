#include "messageservice.h"
#include <slots/data/slotsdatacenter.h>

BEGIN_NAMESPACE(slots)

MessageService::MessageService(){
}

MessageService::~MessageService(){
}

bool MessageService::doJob(CPacket &packet, CResponse &resp){
    int32_t gType;
    if (!getIntVal(packet, "type", gType)) {
        return false;
    }
    SBuf bf;
    ResultFormatter rf(bf);
    bool ret = false;
    switch(gType){
    case 1:{
        ret = getLoginReward(packet);
        rf.formatSimpleResult(ret, "");
        break;
    }
    default:
        break;
    }
    resp.setBody(bf.GetString());
    return ret;
}

bool MessageService::getLoginReward(CPacket &packet) {
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
    return true;
}

END_NAMESPACE
