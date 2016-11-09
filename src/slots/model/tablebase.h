#ifndef TABLEBASE_H
#define TABLEBASE_H

#include <bitset>
#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

#define MAX_BIT_COUNT 32

template<typename T>
struct MutableField{
  T val;
  bool changed;
 MutableField(T &t):val(t), changed(false){}
 MutableField():changed(false){}
  void setVal(T &t) {
    val = t;
    changed = true;
  }
  void setVal(const T &t) {
    val = t;
    changed = true;
  }
  T &operator= (const T&t) {
    val = t;
    changed = true;
    return val;
  }
  T &operator+= (const T&t) {
    val += t;
    changed = true;
    return val;
  }

  bool operator == (const MutableField &right) {
    return val == right.val;
  }

  bool operator > (const T&t) const {
    return val > t;
  }

  bool operator >= (const T&t) const {
    return val >= t;
  }

  bool operator < (const T&t) const {
    return val < t;
  }

  bool operator <= (const T&t) const {
    return val <= t;
  }
};

class TableBase{
public:
  TableBase();
  virtual ~TableBase();

  bool changed() {
    return _fieldStatus.any();
  }

  void resetFieldStatus() {
    _fieldStatus.reset();
  }

protected:
  bool testFieldStatus(int fieldIndex) {
    return _fieldStatus.test(fieldIndex);
  }

  void setFieldStatus(int fieldIndex, bool val) {
    _fieldStatus.set(fieldIndex, val);
  }

  std::bitset<MAX_BIT_COUNT> _fieldStatus; // 1:changed, 0:unchanged
};

#undef MAX_BIT_COUNT

END_NAMESPACE
#endif
