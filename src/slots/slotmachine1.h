#ifndef SLOTSMACHINE1_H
#define SLOTSMACHINE1_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct EleChance {
  int32_t eleId;
  int32_t begin;
  int32_t end;
};

struct GridConfig {
  std::vector<EleChance> elements;
  int32_t totalWeight;
};

/* struct LineConfig { */
/*   std::vector<int32_t> line;// line[0] = ele_position */
/* }; */
typedef std::vector<int32_t> LineConfig;

struct SlotMachineConfig {
  int32_t row;
  int32_t col;
  std::vector<GridConfig> gridsConfig;
  std::vector<LineConfig> linesConfig;
};

class SlotMachine1{
public:
  explicit SlotMachine1(SlotMachineConfig &cfg);
  ~SlotMachine1();

  void play();
  void chooseElement(GridConfig &cfg);
  void countLines();
  void newLine(int32_t eleID);

private:
  SlotMachineConfig &_cfg;
  std::vector<int32_t> _result;
  std::map<int32_t, int32_t> _lineInfo;
};

END_NAMESPACE
#endif
