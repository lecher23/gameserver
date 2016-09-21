#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#include <mysql/mysqlconnpool.h>
#include <slots/slotsconfig.h>
#include <util/config.h>
#define private public
#include <slots/slotmachine1.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testSlotMachine1: public CxxTest::TestSuite 
{
public:
    testSlotMachine1():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_countLines () {
      SlotMachineConfig cfg;
      std::vector<int32_t> line1 = {0,1,2,3};
      std::vector<int32_t> line2 = {8,5,2,7};
      cfg.lines.push_back(line1);
      cfg.lines.push_back(line2);
      SlotMachine1 game(cfg);
      std::map<int32_t, int32_t> result =
              {{0,6}, {1,5}, {2,4}, {3,3},
               {4,3}, {5,4}, {6,3}, {7,4},
               {8,4}, {9,1}, {10,3}, {11,2}};
      GameResultData grd;
      grd.gridsData.swap(result);
      ast_eq(12, grd.gridsData.size());
      game.countLines(grd);
      ast_eq(1, grd.lines.size());
      ast_eq(1, grd.lines[4]);
    }

    void test_chooseElement()
    {
      SlotMachineConfig cfg;
      SlotMachine1 game(cfg);

      SlotGrid sg;
      sg.totalWeight = 10;
      EleChance ec;
      ec.eleID = 10001;
      ec.begin = 0;
      ec.end = 10;
      sg.elements.push_back(ec);

      GameResultData data;
      game.chooseElement(3, sg, data);
      ast_eq(1, data.gridsData.size());
      ast_eq(10001, data.gridsData[3]);
    }

    void test_play()
    {
      ast_true(Config::getInstance().initConfig(cfg_file));
      ast_true(MysqlConnPool::getInstance().init());
      ast_true(SlotsConfig::getInstance().init());
      SlotMachine1 game(SlotsConfig::getInstance().slotConfig);
      GameResultData data;
      game.play(data);
      ast_true(data.gridsData.size() > 0);
      for (auto &item: data.gridsData) {
        CLOG(INFO) << item.first << "," << item.second;
      }
      for (auto &item: data.lines) {
        CLOG(INFO) << item.first << "," << item.second;
      }
    }
private:
    bool _inited;
};
