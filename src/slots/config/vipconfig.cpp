#include "vipconfig.h"

BEGIN_NAMESPACE(slots)

VipConfig::VipConfig(){
}

VipConfig::~VipConfig(){
}

const VipConfigItem &VipConfig::getVipConfigInfo(int32_t vipLevel) {
    static const VipConfigItem defaultVipConfigItem;

    auto itr = config.find(vipLevel);
    if (itr == config.end()) {
        return defaultVipConfigItem;
    }
    return itr->second;
}


bool VipConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sLevel = "Level";
    static const std::string sExpPer = "ExpPer";
    static const std::string sGoldPer = "GoldPer";
    static const std::string sLoginPer = "LogReward";
    static const std::string sPointNeed = "VipPoint";
    static const std::string sMaxBet = "Bet";

    CHECK_MEMBER_IS_ARRAY(doc, "object");
    rapidjson::Value::MemberIterator itr;
    for (size_t i = 0; i < doc.Size(); ++i) {
        GET_MEMBER_IN_DOC(doc[i], itr, sLevel, levelVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sExpPer, expVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sGoldPer, goldVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sLoginPer, loginVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sPointNeed, pointVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sMaxBet, betVal);
        auto level = levelVal.GetInt();
        config[level].level = level;
        config[level].expExtra = expVal.GetDouble();
        config[level].bonusExtra = goldVal.GetDouble();
        config[level].loginExtra = loginVal.GetDouble();
        config[level].pointNeed = pointVal.GetInt();
        config[level].maxBet = betVal.GetInt();
    }
    return true;
}


END_NAMESPACE
