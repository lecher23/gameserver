#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

enum SlotsStyle{
    ESS_SLOTS_1 = 0,
    ESS_END
};

#define SLOTS_GAME_TYPES ESS_END
#define MAX_ELE_TYPES 16

struct GameHistory{
    std::string uid;
    int32_t friendNum;
    int32_t friendGiftsNum;
    int64_t lastLogin;
    int32_t consitiveLogin;
    int32_t tinyGameTimes;
    int32_t bigwin[SLOTS_GAME_TYPES];// 0,0,0,0
    int32_t megawin[SLOTS_GAME_TYPES];// 0,0,0,0
    int32_t freeTimes[SLOTS_GAME_TYPES];
    int32_t gameTimes[SLOTS_GAME_TYPES];
    int32_t jackpotTimes[SLOTS_GAME_TYPES];
    int32_t gJackpotTimes;
    int32_t fourLine[SLOTS_GAME_TYPES][MAX_ELE_TYPES]; // [gametype][ele]
    int32_t fiveLine[SLOTS_GAME_TYPES][MAX_ELE_TYPES]; // [gametype][ele]
    bool changed;
    void reset() {
	uid = uid;
	friendNum = 0;
	friendGiftsNum = 0;
        lastLogin = 0;
	consitiveLogin = 0;
	tinyGameTimes = 0;
	gJackpotTimes = 0;
	for (size_t i = 0; i < SLOTS_GAME_TYPES; ++i){
	    bigwin[i] = 0;
	    megawin[i] = 0;
	    freeTimes[i] = 0;
	    gameTimes[i] = 0;
	    jackpotTimes[i] = 0;
	    for (size_t j = 0; j < MAX_ELE_TYPES; ++j){
		fourLine[i][j] = 0;
		fiveLine[i][j] = 0;
	    }
	}
	changed = true;
    }

    bool deserialize(std::vector<std::string> &row) {
	if (row.size() < 14) return false;
	uid = row[0];
	friendNum = cgserver::StringUtil::StrToInt32WithDefault(row[1].c_str(), 0);
	friendGiftsNum = cgserver::StringUtil::StrToInt32WithDefault(row[2].c_str(), 0);
	lastLogin = cgserver::StringUtil::StrToInt64WithDefault(row[3].c_str(), 0);
	consitiveLogin = cgserver::StringUtil::StrToInt32WithDefault(row[4].c_str(), 0);
	tinyGameTimes = cgserver::StringUtil::StrToInt32WithDefault(row[5].c_str(), 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[6].c_str(), ',', bigwin, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[7].c_str(), ',', megawin, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[8].c_str(), ',', freeTimes, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[9].c_str(), ',', gameTimes, SLOTS_GAME_TYPES, 0);
	cgserver::StringUtil::StrToIntArrayWithDefault
	    (row[10].c_str(), ',', jackpotTimes, SLOTS_GAME_TYPES, 0);
	gJackpotTimes = cgserver::StringUtil::StrToInt32WithDefault(row[11].c_str(), 0);
	int32_t *pFourLine[SLOTS_GAME_TYPES];
	int32_t *pFiveLine[SLOTS_GAME_TYPES];
	for (size_t i = 0; i < SLOTS_GAME_TYPES; ++i) {
	    pFourLine[i] = fourLine[i];
	    pFiveLine[i] = fiveLine[i];
	}
	cgserver::StringUtil::StrToIntArray
	    (row[12].c_str(), ';', ',', pFourLine, SLOTS_GAME_TYPES,MAX_ELE_TYPES);
	cgserver::StringUtil::StrToIntArray
	    (row[13].c_str(), ';', ',', pFiveLine, SLOTS_GAME_TYPES,MAX_ELE_TYPES);

	return true;
    }
    GameHistory() {
        reset();
    }
};
DF_SHARED_PTR(GameHistory);

END_NAMESPACE
#endif
