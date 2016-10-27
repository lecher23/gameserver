#include "sloteleconfig.h"
#include "util/stringutil.h"

BEGIN_NAMESPACE(slots)

SlotEleConfig::SlotEleConfig(){
}

SlotEleConfig::~SlotEleConfig(){
}

int32_t SlotEleConfig::begin() {
    return 0;
}

int32_t SlotEleConfig::end() {
    return _cfg.size();
}

const RawElementItem &SlotEleConfig::getElement(int index) {
    return _cfg[index];
}

bool SlotEleConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sRepeat = "Repeat";
    static const std::string sLinkPrefix = "Link";

    rapidjson::Value::MemberIterator itr;
    for (int i = 0; i < doc.Capacity(); ++i) {
        RawElementItem item;
        for (itr = doc[i].MemberBegin(); itr != doc[i].MemberEnd(); ++itr){
            std::string key(itr->name.GetString());
            if (key == sRepeat) {
                item.repeatable = (itr->value.GetInt() == 1);
            }else if (key == JsonConfigStr::sID) {
                item.id = itr->value.GetInt();
            }else if (key == JsonConfigStr::sType) {
                item.type = itr->value.GetInt();
            }else if (key.compare(0, sLinkPrefix.size(), sLinkPrefix) == 0) {
                int32_t linkVal;
                if (!cgserver::StringUtil::StrToInt32(
                        key.substr(sLinkPrefix.size()).data(), linkVal)){
                    CLOG(ERROR) << "Invalid Link key name:" << key;
                    return false;
                }
                item.reward[linkVal] = itr->value.GetInt();
            }
        }
        _cfg.push_back(item);
    }
    return true;
}

END_NAMESPACE
