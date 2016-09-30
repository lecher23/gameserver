#include <slots/slotsconfig.h>
#include <slots/data/slotsdatacenter.h>
#include <mysql/mysqlconnpool.h>

using namespace cgserver;
using namespace slots;

#define INIT_GRID(row, col, i1, w1, i2, w2)                \
    {                                                      \
        auto &grid = ts_cfg.getGrid(row, col, false);      \
        grid.addElement(i1, w1);                           \
        grid.addElement(i2, w2);                           \
    }

#define INIT_LINE(x, y, z, id)                  \
    {                                           \
        std::vector<int32_t> tmp = {x,y,z};     \
        ts_cfg.addLine(id, tmp);                \
    }

#define INIT_ELEMENT(id, type, repeat, b1, b2)  \
    {                                           \
        ts_cfg.addElement(id, type, repeat);       \
        ts_cfg.setElementRatio(id, 2, b1);    \
        ts_cfg.setElementRatio(id, 3, b2);    \
    }

class CommonTools {
public:
    static void init_slots_config_tsconfig() {
        auto &cfg = SlotsConfig::getInstance();
        auto &ts_cfg = cfg.themeConfig.tsConfig;
        ts_cfg.setRowNumber(2);
        ts_cfg.setColumnNumber(3);
        // add grid info
        INIT_GRID(0, 0, 1001, 3, 1002, 6);
        INIT_GRID(0, 0, 1003, 1, 1004, 1);
        INIT_GRID(0, 0, 1005, 1, 1006, 1);

        INIT_GRID(0, 1, 1001, 3, 1002, 6);
        INIT_GRID(0, 1, 1003, 2, 1004, 4);
        INIT_GRID(0, 1, 1005, 1, 1006, 1);

        INIT_GRID(0, 2, 1001, 3, 1002, 6);
        INIT_GRID(0, 2, 1003, 2, 1004, 4);
        INIT_GRID(0, 2, 1005, 1, 1006, 1);

        INIT_GRID(1, 0, 1001, 3, 1002, 6);
        INIT_GRID(1, 0, 1003, 2, 1004, 4);
        INIT_GRID(1, 0, 1005, 1, 1006, 1);

        INIT_GRID(1, 1, 1001, 3, 1002, 6);
        INIT_GRID(1, 1, 1003, 2, 1004, 4);
        INIT_GRID(1, 1, 1005, 1, 1006, 1);

        INIT_GRID(1, 2, 1001, 3, 1002, 6);
        INIT_GRID(1, 2, 1003, 2, 1004, 4);
        INIT_GRID(1, 2, 1005, 1, 1006, 1);
        // add line info
        INIT_LINE(0, 1, 2, 999);
        INIT_LINE(3, 4, 5, 888);
        INIT_LINE(3, 1, 5, 777);
        INIT_LINE(0, 4, 5, 666);
        INIT_LINE(3, 1, 2, 555);
        // add element info
        INIT_ELEMENT(1001, 0, true, 10, 20);
        INIT_ELEMENT(1002, 0, true, 30, 80);
        INIT_ELEMENT(1003, 1, false, 0, 0);
        INIT_ELEMENT(1004, 0, false, 0, 0);
        INIT_ELEMENT(1005, 0, false, 0, 0);
        INIT_ELEMENT(1006, 0, false, 0, 0);
        // set common info
        ts_cfg.setTax4Room(10);
        ts_cfg.setTax4Hall(5);
        ts_cfg.setMinRoomPrizePool(5000);
        ts_cfg.setJackpot1EleID(1005);
        ts_cfg.setJackpot1Count(3);
        ts_cfg.setRoomReserveTime(1);
        ts_cfg.setForceWinRoomPrize(10);
        ts_cfg.setJackpot1Limit(100);
        ts_cfg.getFreeGameConfig().setElementId(1004);
        ts_cfg.getTinyGameConfig().setElementId(1005);
        ts_cfg.setBigwin(30);
        ts_cfg.setMegawin(80);
        ts_cfg.setSuperwin(100);
    }
#undef INIT_GRID
#undef INIT_LINE
#undef INIT_ELEMENT
    static void init_slots_data_center_data() {
        auto &sdc = slots::SlotsDataCenter::instance();
        sdc.slotsUserData.reset(new SlotsUserData);
        sdc.cjQueue.reset(new CjQueue);
        // hall pool, room pool, reserve time
        sdc.getHall(1).init(10000, 5000, 1);
    }
};
