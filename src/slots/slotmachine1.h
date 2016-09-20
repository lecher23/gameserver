#ifndef SLOTSMACHINE1_H
#define SLOTSMACHINE1_H

#include <slots/data/slotmachineconfig.h>

BEGIN_NAMESPACE(slots)

class SlotMachine1{
public:
  explicit SlotMachine1(SlotMachineConfig &cfg);
  ~SlotMachine1();

  void play();
  void chooseElement(int32_t idx, SlotGrid &cfg);
  void countLines();
  void newLine(int32_t eleID);

private:
  SlotMachineConfig &_cfg;
  std::vector<int32_t> _result;
  std::map<int32_t, int32_t> _lineInfo;
};

END_NAMESPACE
#endif
