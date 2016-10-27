#include "slotlinesconfig.h"
#include "util/stringutil.h"

BEGIN_NAMESPACE(slots)

SlotLinesConfig::SlotLinesConfig(){
}

SlotLinesConfig::~SlotLinesConfig(){
}

int32_t SlotLinesConfig::getPointCount() {
    return pointCount;
}

int32_t SlotLinesConfig::begin(){
    return 0;
}

int32_t SlotLinesConfig::end(){
    return _lines.size();
}

const std::vector<int32_t> &SlotLinesConfig::getLine(int32_t index) {
    return _lines[index];
}

bool SlotLinesConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sColumnPrefix = "Column";
    rapidjson::Value::MemberIterator itr;
    for (itr = doc[0].MemberBegin(); itr != doc[0].MemberEnd(); ++itr){
        std::string key(itr->name.GetString());
        if (key.compare(0, sColumnPrefix.size(), sColumnPrefix) == 0) {
            ++pointCount;
        }
    }
    for (int i = 0; i < doc.Capacity(); ++i) {
        int32_t rowPos;
        std::vector<int32_t> line;
        for (int j = 0; j < pointCount; ++j) {
            std::string key = sColumnPrefix + cgserver::StringUtil::toString(j);
            GET_INT32_MEMBER_IN_DOC(doc[i], itr, key, rowPos);
            line.push_back(j + rowPos * pointCount);
        }
        _lines.push_back(line);
    }
    return true;
}

END_NAMESPACE
