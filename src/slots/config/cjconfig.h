#ifndef CJCONFIG_H
#define CJCONFIG_H

#include <slots/config/jsonconfigbase.h>

BEGIN_NAMESPACE(slots)

#define POWERFUL_HALL_ID 0

struct CjConfigItem{
  int32_t cjID;
  int32_t hallID;
  int32_t rewardValue;
  int64_t target;
  CjConfigItem(){}
 CjConfigItem(int32_t cj_id, int32_t hall_id, int32_t rd_val, int64_t t):
  cjID(cj_id), hallID(hall_id), rewardValue(rd_val), target(t) {}
};

typedef std::vector<CjConfigItem> CjConfigGrp;

class CjConfig: public JsonConfigBase{
public:
  CjConfig();
  ~CjConfig();

  void getCjID(
      int32_t cjType, int32_t hall, int64_t pre, int64_t cur, std::vector<int32_t> &out);
  const CjConfigItem &getCjInfo(int32_t cjID);

protected:
  virtual bool parseJsonDoc(rapidjson::Document &doc);

private:
  std::map<int32_t, CjConfigGrp> config; // key: cj type
};

END_NAMESPACE
#endif
