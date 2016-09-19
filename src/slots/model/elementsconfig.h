#ifndef ELEMENTSCONFIG_H
#define ELEMENTSCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct SlotElement{
  int32_t id;
  int32_t type;
  bool deserialize(std::vector<std::string> &row) {
    if (row.size() < 2) {
      CLOG(ERROR) << "invalid fields num";
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[0].c_str(), id);
    ret = ret && cgserver::StringUtil::StrToInt32(row[1].c_str(), type);
    return ret;
  }
};

struct ElementConfig {
  int32_t eleID;
  int32_t lineNum;
  int64_t value;
  bool deserialize(std::vecotr<std::string> &row) {
    if(row.size() < 4) {
      CLOG(ERROR) << "invalid fields num";
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[1], eleID);
    ret = ret && cgserver::StringUtil::StrToInt32(row[2], lineNum);
    ret = ret && cgserver::StringUtil::StrToInt32(row[3], value);
    return ret;
  }
};


END_NAMESPACE
#endif
