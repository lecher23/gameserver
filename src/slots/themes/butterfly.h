#ifndef BUTTERFLY_H
#define BUTTERFLY_H

#include <util/common_define.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>

BEGIN_NAMESPACE(slots)

class Butterfly{
public:
  Butterfly();
  ~Butterfly();

  int32_t play(int32_t selectCount, int32_t &multiple, std::vector<int32_t> &ret) const;
  void addFactor(int32_t value);
  bool initFromJsonFile(const std::string &path);

private:
  bool parseJsonFile(const char *str);

  // element at index 0 is special element.
  int32_t _id{1};
  std::vector<int32_t> factors;
};

END_NAMESPACE
#endif
