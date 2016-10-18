#include "updateservice.h"
#include "slots/data/slotsdatacenter.h"
#include "slots/resultformatter.h"

BEGIN_NAMESPACE(slots)
UpdateService::UpdateService(){
}

UpdateService::~UpdateService(){
}

bool UpdateService::doJob(CPacket &packet, CResponse &resp){
    std::string type;
    GET_PARAM(slotconstants::sType, type, true);
    SBuf buf;
    ResultFormatter rf(buf);
    if (type == "1" ) {
        auto ret = updateUserInfo(packet);
        rf.formatSimpleResult(ret, "");
        resp.setBody(buf.GetString());
    }
    return true;
}

#define SET_USER_INFO_ATTR(tmp, key, target)    \
    GET_PARAM(key, tmp, false);                 \
    if (!tmp.empty()) {                         \
        target = tmp;                           \
        tmp.clear();                            \
    }

bool UpdateService::updateUserInfo(CPacket &packet) {
    std::string uid;
    GET_PARAM(slotconstants::sUserID, uid, true);

    SlotsUserPtr user;
    if(!SlotsDataCenter::instance().slotsUserData->getByUid(uid, user)) {
        return false;
    }

    auto &uInfo = user->uInfo;
    std::string tmp;
    SET_USER_INFO_ATTR(tmp, slotconstants::sMale, uInfo.male);
    SET_USER_INFO_ATTR(tmp, slotconstants::sCountry, uInfo.country);
    SET_USER_INFO_ATTR(tmp, slotconstants::sName, uInfo.fname);
    SET_USER_INFO_ATTR(tmp, slotconstants::sAvatar, uInfo.avatar);
    return true;
}
#undef SET_USER_INFO_ATTR

END_NAMESPACE
