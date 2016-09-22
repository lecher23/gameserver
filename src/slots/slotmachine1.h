#ifndef SLOTSMACHINE1_H
#define SLOTSMACHINE1_H

#include <slots/data/slotmachineconfig.h>
#include <slots/data/basicdata.h>
#include <set>

BEGIN_NAMESPACE(slots)

class SlotMachine1{
public:
  explicit SlotMachine1(SlotMachineConfig &cfg);
  ~SlotMachine1();

  void play(GameResultData &data) const ;
  void chooseElementInColumn(int32_t column, GameResultData &data) const;
  int32_t locateElement(int32_t rd, std::set<int32_t> &forbid, SlotGrid &grid) const;

  void countLines(GameResultData &data) const;
  void collectSpecial(GameResultData &data) const;

private:
  SlotMachineConfig &_cfg;
};

END_NAMESPACE
#endif
