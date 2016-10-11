#ifndef JSONCONFIGBASE_H
#define JSONCONFIGBASE_H

#include <util/common_define.h>
#include <util/file_util.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

BEGIN_NAMESPACE(slots)

#define GET_MEMBER_IN_DOC(doc, itr, mem, dest)          \
  itr = doc.FindMember(mem.c_str());                    \
  if (itr == doc.MemberEnd()) {                         \
    CLOG(ERROR) << "Member [" << mem << "] required.";  \
    return false;                                       \
  }                                                     \
  rapidjson::Value &dest = itr->value;

#define CHECK_MEMBER_IS_ARRAY(value, tag)               \
  if (!value.IsArray()) {                               \
    CLOG(ERROR) << "["<< tag <<"] should be array.";    \
    return false;                                       \
  }

class JsonConfigBase{
public:
  JsonConfigBase() {}
  ~JsonConfigBase() {}

  bool initFromJsonFile(const std::string &path) {
    return cgserver::processFile(
        path, std::bind(
            &JsonConfigBase::parseJsonFile, this, std::placeholders::_1));
  }

protected:
  bool parseJsonFile(const char *str) {
    rapidjson::Document doc;
    doc.Parse<0>(str);
    if (doc.HasParseError()) {
        CLOG(ERROR) << "parse config failed:" << doc.GetParseError();
        return false;
    }
    if (doc.IsArray() && doc.Size() == 0) {
        CLOG(ERROR) << "no obj in config file";
        return false;
    }
    return parseJsonDoc(doc);
  }

  virtual bool parseJsonDoc(rapidjson::Document &doc) = 0;
};

END_NAMESPACE
#endif
