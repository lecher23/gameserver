#include "cjconfig.h"

BEGIN_NAMESPACE(slots)

CjConfig::CjConfig(){
}

CjConfig::~CjConfig(){
}

const CjConfigItem &CjConfig::getCjInfo(int32_t cjID) {
    static const CjConfigItem defaultCjItem(0,0,0,0);
    for (auto &item: config) {
        for (auto &cjItem: item.second) {
            if (cjItem.cjID == cjID) {
                return cjItem;
            }
        }
    }
    return defaultCjItem;
}

void CjConfig::getCjID(
    int32_t cjType, int32_t hallID, int64_t pre, int64_t cur, std::vector<int32_t> &cjs)
{
    cjs.clear();
    auto itr = config.find(cjType);
    if (itr == config.end()) {
        return;
    }
    for (auto &item: itr->second) {
        if (item.hallID != POWERFUL_HALL_ID && item.hallID != hallID) {
            continue;
        }
        if (pre < item.target && cur >= item.target) {
            cjs.push_back(item.cjID);
        }
    }
}

bool CjConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sID = "ID";
    static const std::string sCjType = "TaTy";
    static const std::string sCjTarget = "TaC1";
    static const std::string sCjReward = "ReTy";
    static const std::string sHallID = "Machine";

    rapidjson::Value::MemberIterator itr;
    CHECK_MEMBER_IS_ARRAY(doc, "");
    for (size_t i = 0; i < doc.Capacity(); ++i) {
        GET_MEMBER_IN_DOC(doc[i], itr, sID, idVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sCjType, cjTypeVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sCjTarget, cjTargetVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sCjReward, rewardVal);
        GET_MEMBER_IN_DOC(doc[i], itr, sHallID, hallVal);
        config[cjTypeVal.GetInt()].push_back(
            CjConfigItem(
                idVal.GetInt(), hallVal.GetInt(),
                rewardVal.GetInt(), cjTargetVal.GetInt64()));
    }
    return true;
}

END_NAMESPACE
