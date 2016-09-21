#ifndef SLOTSMACHINE1_H
#define SLOTSMACHINE1_H

#include <slots/data/slotmachineconfig.h>
#include <slots/data/basicdata.h>

BEGIN_NAMESPACE(slots)

class SlotMachine1{
public:
  explicit SlotMachine1(SlotMachineConfig &cfg);
  ~SlotMachine1();

  void play(GameResultData &data) const ;
  void chooseElement(int32_t idx, SlotGrid &cfg, GameResultData &data) const ;
  void countLines(GameResultData &data) const;
  void collectSpecial(GameResultData &data) const;

private:
  SlotMachineConfig &_cfg;
};

END_NAMESPACE
#endif
