#include "butterfly.h"

#include <util/random.h>
#include <util/file_util.h>

BEGIN_NAMESPACE(slots)

Butterfly::Butterfly(){
}

Butterfly::~Butterfly(){
}

int32_t Butterfly::play(
    int32_t selectCount, int32_t &multiple, std::vector<int32_t> &ret) const
{
    int32_t result = 0;
    std::vector<int32_t> helper(factors.size());
    for (size_t i = 0; i < factors.size(); ++i) {
        helper[i] = i;
    }
    multiple = 1;
    ret.clear();
    cgserver::Random::getInstance().randMutiVal(selectCount, helper);

    for (size_t i = 0; i < selectCount; ++i) {
        if (helper[i] == 0) {
            multiple = factors[helper[i]];
        }else{
            result += factors[helper[i]];
            ret.push_back(factors[helper[i]]);
        }
    }
    return result * multiple;
}

void Butterfly::addFactor(int32_t val) {
    factors.push_back(val);
}

bool Butterfly::initFromJsonFile(const std::string &path) {
    return cgserver::processFile(
        path, std::bind(
            &Butterfly::parseJsonFile, this, std::placeholders::_1));
}

bool Butterfly::parseJsonFile(const char *str) {
    rapidjson::Document doc;
    doc.Parse<0>(str);
    if (doc.HasParseError()) {
        CLOG(ERROR) << "parse butterfly config failed:" << doc.GetParseError();
        return false;
    }
    if (doc.Size() == 0) {
        CLOG(ERROR) << "no obj in butterfly config file";
        return false;
    }
    rapidjson::Value& item = doc[0];
    _id = item["ID"].GetInt();
    addFactor(item["Special"].GetInt());
    rapidjson::Value& ary = item["Bonus"];
    if(ary.IsArray()) {
        for (size_t i = 0; i < ary.Capacity(); ++i) {
            addFactor(ary[i].GetInt());
        }
    }else {
        CLOG(ERROR) << "cannot found array in [Bonus]";
        return false;
    }
    return true;
}

END_NAMESPACE
