#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

namespace ThemeHistoryStr{
  const std::string sTableName = "theme_history";
  const std::string sPriKey1 = "uid";
  const std::string sPriKey2 = "theme_id";
  const std::string sPriKey3 = "tag";
  const std::string sValue = "value";
}

namespace GameHistoryStr{
  const std::string sTableName = "game_history";
  const std::string sPriKey = "uid";
  const std::string sMaxFortune = "max_fortune";
  const std::string sMaxEarned = "max_earned";
  const std::string sTotalEarned = "total_earned";
  const std::string sTotalBet = "total_bet";
  const std::string sLastLogin = "last_login";
  const std::string sLoingDays = "login_days";
  const std::string sJackpot = "jackpot";
};

enum ThemeTypes{
  ESS_UNKNOW = 0,
  ESS_TEMPLE_PRINCESS = 1,
  ESS_END
};

enum ThemeHistoryTag {
  THEME_TAG_BEGIN = 0,
  BIG_WIN_TAG = 1,
  MEGA_WIN_TAG,
  SUPER_WIN_TAG,
  NORMAL_GAME_TAG,
  FREE_GAME_TAG,
  TINY_GAME_TAG,
  SIX_LINK_TAG,
  JACKPOT_TAG,
  INVALID_THEME_TAG
};

#define THEME_TYPES ESS_END
#define THEME_HISTORY_TAGS INVALID_THEME_TAG

class ThemeHistory {
 public:
  ThemeHistory() {
    reset();
  }

  void reset() {
    for (int i = 0; i < THEME_HISTORY_TAGS; ++i) {
      for (int j = 0; j < THEME_TYPES; ++j) {
        history[j][i] = 0;
      }
    }
  }

  int32_t getThemeCount() {
    return THEME_TYPES;
  }

  int32_t getTagCount() {
    return THEME_HISTORY_TAGS;
  }

  bool deserialize (const std::vector<std::vector<std::string>> &rows) {
    for (auto &row: rows) {
      if (!deserialize(row)) {
        return false;
      }
    }
    return true;
  }

  bool deserialize(const std::vector<std::string> &row) {
    if (row.size() < 4) return false;
    int32_t themeType;
    bool ret = cgserver::StringUtil::StrToInt32(row[1].c_str(), themeType);
    if (!ret || themeType >= ESS_END) {
      return false;
    }
    int32_t recordType;
    ret = cgserver::StringUtil::StrToInt32(row[2].c_str(), recordType);
    if (!ret || recordType >= INVALID_THEME_TAG) {
      return false;
    }
    int32_t value;
    if (!cgserver::StringUtil::StrToInt32(row[3].c_str(), value)) {
      return false;
    }
    setTagValue(themeType, recordType, value);
    return true;
  }

  void setTagValue(int32_t themeType, int32_t tag, int32_t val) {
    if (tag >= THEME_HISTORY_TAGS || themeType >= THEME_TYPES ) {
      return;
    }
    history[themeType][tag] = val;
  }

  void incrTagValue(int32_t themeType, int32_t tag, int32_t val) {
    if (tag >= THEME_HISTORY_TAGS || themeType >= THEME_TYPES ) {
      return;
    }
    history[themeType][tag] += val;
  }

  int32_t getTagValue(int32_t themeType, int32_t tag) {
    if (tag >= THEME_HISTORY_TAGS || themeType >= THEME_TYPES ) {
      return 0;
    }
    return history[themeType][tag];
  }
 private:
  int32_t history[THEME_TYPES][THEME_HISTORY_TAGS];
  bool changed{false};
};

struct ThemeHistoryItem{
  int32_t bigWinCount{0};
  int32_t megaWinCount{0};
  int32_t superWinCount{0};
  int32_t spinCount{0};
  int32_t freeGameCount{0};
  int32_t tinyGameCount{0};
  int32_t maxLinkCount{0};
};

struct GameHistory{
  int32_t uid{0};
  int64_t maxFortune{0};
  int64_t maxEarned{0};
  int64_t totalEarned{0};
  int64_t totalBet{0};
  int64_t lastLogin{0};
  int32_t loginDays{0};
  int32_t jackpot{0};
  ThemeHistory themeHistory;
  bool changed{false};

  void reset() {
    maxFortune = 0;
    maxEarned = 0;
    totalEarned = 0;
    totalBet = 0;
    lastLogin = 0;
    loginDays = 0;
    jackpot = 0;
    themeHistory.reset();
  }

  bool deserialize(const std::vector<std::string> &row) {
    if (row.size() < 8) return false;
    bool ret = cgserver::StringUtil::StrToInt64(row[1].c_str(), maxFortune);
    ret = ret && cgserver::StringUtil::StrToInt64(row[2].c_str(), maxEarned);
    ret = ret && cgserver::StringUtil::StrToInt64(row[3].c_str(), totalEarned);
    ret = ret && cgserver::StringUtil::StrToInt64(row[4].c_str(), totalBet);
    ret = ret && cgserver::StringUtil::StrToInt64(row[5].c_str(), lastLogin);
    ret = ret && cgserver::StringUtil::StrToInt32(row[6].c_str(), loginDays);
    ret = ret && cgserver::StringUtil::StrToInt32(row[7].c_str(), jackpot);
    return ret;

  }

  void incrBet(int64_t bet) {
    totalBet += bet;
    changed = true;
  }

  void newFortune(int64_t fortune) {
    if (fortune > maxFortune) {
      maxFortune = fortune;
      changed = true;
    }
  }

  void newEarned(int64_t earned) {
    if (maxEarned < earned) {
      maxEarned = earned;
      changed = true;
    }
  }

  void incrEarned(int64_t earned) {
    if (earned == 0) return;
    totalEarned += earned;
    changed = true;
  }

};
DF_SHARED_PTR(GameHistory);

END_NAMESPACE
#endif
