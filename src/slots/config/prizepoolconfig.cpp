#include "prizepoolconfig.h"

BEGIN_NAMESPACE(slots)

PrizePoolConfig::PrizePoolConfig(){
}

PrizePoolConfig::~PrizePoolConfig(){
}

const PoolConfigItem &PrizePoolConfig::getHallConfig() {
    return _hallConfig;
}

const PoolConfigItem &PrizePoolConfig::getRoomConfig() {
    return _roomConfig;
}


bool PrizePoolConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sLimitBet = "LimitBet";
    static const std::string sTax = "PerBonus";
    static const std::string sBase = "BaseBonus";

    rapidjson::Value::MemberIterator itr;
    if (doc.Capacity() < 2) {
        CLOG(ERROR) << "Lack config item.";
        return false;
    }
    GET_INT32_MEMBER_IN_DOC(doc[0], itr, JsonConfigStr::sID, _roomConfig.id);
    GET_INT32_MEMBER_IN_DOC(doc[0], itr, sLimitBet, _roomConfig.minBet);
    int32_t tmp;
    GET_INT32_MEMBER_IN_DOC(doc[0], itr, sTax, tmp);
    _roomConfig.tax = tmp/100.0;
    GET_MEMBER_ITR_IN_DOC(doc[0], itr, sBase);
    _roomConfig.base = itr->value.GetInt64();

    GET_INT32_MEMBER_IN_DOC(doc[1], itr, JsonConfigStr::sID, _hallConfig.id);
    GET_INT32_MEMBER_IN_DOC(doc[1], itr, sLimitBet, _hallConfig.minBet);
    GET_INT32_MEMBER_IN_DOC(doc[1], itr, sTax, tmp);
    _hallConfig.tax = tmp/100.0;
    GET_MEMBER_ITR_IN_DOC(doc[1], itr, sBase);
    _hallConfig.base = itr->value.GetInt64();

    return true;
}

END_NAMESPACE
