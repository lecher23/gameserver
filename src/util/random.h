#ifndef RANDOM_H
#define RANDOM_H

#include <time.h>
#include <stdlib.h>
#include <array>
#include <utility>
#include <util/common_define.h>

BEGIN_NAMESPACE(cgserver)

class Random{
public:
  ~Random() {
  }
  static Random &getInstance() {
    static Random rd;
    return rd;
  }

  // [begin, end)
  int32_t randGap(int32_t begin, int32_t end) {
    return rand() % (end - begin) + begin;
  }

  void randMutiVal(int32_t wanted, std::vector<int32_t> &data)
  {
    for (int i = 0; i < wanted; ++i) {
      auto x = randGap(i, data.size());
      std::swap(data[i], data[x]);
    }
  }

private:
  Random() {
    srand(time(nullptr));
  }

  Random(const Random &rd);
};

END_NAMESPACE
#endif
