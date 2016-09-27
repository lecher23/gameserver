#ifndef BET2EXPCONFIG_H
#define BET2EXPCONFIG_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

class Bet2ExpConfigs {
public:
  void addConfig(int64_t bet, int64_t exp) {
    _cfg[bet] = exp;
  }

  int64_t getExp(int64_t bet) {
    auto itr = _cfg.find(bet);
    if(itr != _cfg.end()) {
      return itr->second;
    }
    return bet;
  }
private:
  std::map<int64_t, int64_t> _cfg;
};

class Bet2ExpConfig{
public:
  static bool deserialize(const std::vector<std::string> &row, Bet2ExpConfigs &out) {
    if (row.size() < 2) {
      CLOG(INFO) << "Invalid col num";
      return false;
    }
    int64_t key;
    bool ret = cgserver::StringUtil::StrToInt64(row[0].c_str(), key);
    int64_t val;
    ret = ret && cgserver::StringUtil::StrToInt64(row[1].c_str(), val);
    if (!ret) {
      CLOG(ERROR) << "Init level config failed.";
    }
    out.addConfig(key, val);
    return ret;
  }
};

END_NAMESPACE
#endif
