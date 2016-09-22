#ifndef TEMPLEPRINCESS_H
#define TEMPLEPRINCESS_H

#include <slots/themes/tsconfig.h>
#include <slots/data/basicdata.h>
#include <set>

BEGIN_NAMESPACE(slots)

#define WILD_ELEMENT_TYPE 2

class TemplePrincess{
public:
  explicit TemplePrincess(TSConfig &cfg);
  ~TemplePrincess();

  bool play(GameResultData &data) const ;
  bool chooseElementInColumn(int32_t column, GameResultData &data) const;
  int32_t locateElement(int32_t rd, std::set<int32_t> &forbid, TSGrid &grid) const;

  void countLines(GameResultData &data) const;

private:
  TSConfig &_cfg;
};

END_NAMESPACE
#endif
