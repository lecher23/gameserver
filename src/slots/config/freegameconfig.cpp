#include "freegameconfig.h"

BEGIN_NAMESPACE(slots)

FreeGameConfig::FreeGameConfig(){
}

FreeGameConfig::~FreeGameConfig(){
}

int32_t FreeGameConfig::begin() {
    return 0;
}

int32_t FreeGameConfig::end() {
    return _cfg.size();
}

const std::pair<int32_t, int32_t> &FreeGameConfig::getItem(int index) {
    return _cfg[index];
}

bool FreeGameConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sEleCount = "Counts";
    static const std::string sFreeTimes = "Numbers";

    rapidjson::Value::MemberIterator itr;
    for (int i = 0; i < doc.Capacity(); ++i) {
        std::pair<int32_t, int32_t> tmp;
        GET_INT32_MEMBER_IN_DOC(doc[i], itr, sEleCount, tmp.first);
        GET_INT32_MEMBER_IN_DOC(doc[i], itr, sFreeTimes, tmp.second);
        _cfg.push_back(tmp);
    }
    return true;
}

END_NAMESPACE
