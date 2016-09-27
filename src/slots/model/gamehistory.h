#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

enum SlotsStyle{
  ESS_UNKNOW = 0,
  ESS_TEMPLE_PRINCESS = 1,
  ESS_END
};

enum ThemeHistoryTag {
  BIG_WIN_TAG = 1,
  MEGA_WIN_TAG,
  SUPER_WIN_TAG,
  FREE_GAME_TAG,
  TINY_GAME_TAG,
  NORMAL_GAME_TAG,
  SIX_LINK_TAG,
  JACKPOT_TAG,
  INVALID_THEME_TAG
};

#define SLOTS_GAME_TYPES ESS_END
#define THEME_HISTORY_TAGS 7

class ThemeHistory {
 public:
  ThemeHistory() {
    for (int i = 0; i < THEME_HISTORY_TAGS; ++i) {
      for (int j = 0; j < SLOTS_GAME_TYPES; ++j) {
        history[j][i] = 0;
      }
    }
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
    if (tag >= THEME_HISTORY_TAGS || themeType >= SLOTS_GAME_TYPES ) {
      return;
    }
    history[themeType][tag] = val;
  }

  void incrTagValue(int32_t themeType, int32_t tag, int32_t val) {
    if (tag >= THEME_HISTORY_TAGS || themeType >= SLOTS_GAME_TYPES ) {
      return;
    }
    history[themeType][tag] += val;
  }

  int32_t getTagValue(int32_t themeType, int32_t tag) {
    if (tag >= THEME_HISTORY_TAGS || themeType >= SLOTS_GAME_TYPES ) {
      return 0;
    }
    return history[themeType][tag];
  }
 private:
  int32_t history[SLOTS_GAME_TYPES][THEME_HISTORY_TAGS];
};

struct GameHistory{
  int32_t uid{0};
  int32_t friendNum{0};
  int32_t friendGiftsNum{0};
  int32_t loginDays{0};
  int32_t jackpot{0};
  int64_t lastLogin{0};
  ThemeHistory themeHistory;
  bool changed;
  void reset() {
    friendNum = 0;
    friendGiftsNum = 0;
    lastLogin = 0;
    loginDays = 0;
    jackpot = 0;
    changed = true;
  }

  bool deserialize(std::vector<std::string> &row) {
    if (row.size() < 6) return false;
    bool ret = cgserver::StringUtil::StrToInt32(row[0].c_str(), uid);
    ret = ret && cgserver::StringUtil::StrToInt32(row[1].c_str(), friendNum);
    ret = ret && cgserver::StringUtil::StrToInt32(row[2].c_str(), friendGiftsNum);
    ret = ret && cgserver::StringUtil::StrToInt64(row[3].c_str(), lastLogin);
    ret = ret && cgserver::StringUtil::StrToInt32(row[4].c_str(), loginDays);
    ret = ret && cgserver::StringUtil::StrToInt32(row[5].c_str(), jackpot);
    return ret;
  }
};
DF_SHARED_PTR(GameHistory);

END_NAMESPACE
#endif
