#ifndef TEMPLEPRINCESS_H
#define TEMPLEPRINCESS_H

#include <slots/themes/tsconfig.h>
#include <slots/data/basicdata.h>
#include <set>

BEGIN_NAMESPACE(slots)

class TemplePrincess{
public:
  explicit TemplePrincess(TSConfig &cfg);
  ~TemplePrincess();

  bool play(TSResult &data) const ;

private:
  bool chooseElementInColumn(int32_t column, int32_t maxRow, TSResult &data) const;
  int32_t locateElement(int32_t rd, std::set<int32_t> &forbid, TSGrid &grid) const;
  void countLines(TSResult &data) const;
  void processLines(TSResult &data) const;
  void processSpecial(TSResult &data) const;
  void playTinyGame(TSResult &data) const;

  TSConfig &_cfg;
};

END_NAMESPACE
#endif
