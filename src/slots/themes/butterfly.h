#ifndef BUTTERFLY_H
#define BUTTERFLY_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

class Butterfly{
public:
  Butterfly();
  ~Butterfly();

  void play(int32_t selectCount, int32_t &multiple, std::vector<int32_t> &ret) const;
  void addFactor(int32_t value);
private:
  // element at index 0 is special element.
  std::vector<int32_t> factors;
};

END_NAMESPACE
#endif
