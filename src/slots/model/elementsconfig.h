#ifndef ELEMENTSCONFIG_H
#define ELEMENTSCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

typedef std::map<int32_t, int32_t> SlotElements;

struct SlotElement{
  static bool deserialize(std::vector<std::string> &row, SlotElements &out) {
    if (row.size() < 2) {
      CLOG(ERROR) << "invalid fields num";
      return false;
    }
    int32_t id, type;
    bool ret = cgserver::StringUtil::StrToInt32(row[0].c_str(), id);
    ret = ret && cgserver::StringUtil::StrToInt32(row[1].c_str(), type);
    if (ret) {
      out[id] = type;
    }
    return ret;
  }
};

struct ElementConfig {
  int32_t eleID;
  int32_t lineNum;
  int64_t value;
  bool deserialize(std::vector<std::string> &row) {
    if(row.size() < 4) {
      CLOG(ERROR) << "invalid fields num";
      return false;
    }
    bool ret = cgserver::StringUtil::StrToInt32(row[1].c_str(), eleID);
    ret = ret && cgserver::StringUtil::StrToInt32(row[2].c_str(), lineNum);
    ret = ret && cgserver::StringUtil::StrToInt64(row[3].c_str(), value);
    return ret;
  }
};

typedef std::vector<ElementConfig> ElementsConfig;

END_NAMESPACE
#endif
