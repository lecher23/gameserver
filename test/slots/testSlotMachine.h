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
      CLOG(INFO) << "test play() done.";
    }

    void test_locateElement_no_forbid()
    {
      std::set<int32_t> forbid;
      SlotGrid sg;
      {
        EleChance ec;
        ec.weight = 5;
        ec.eleID = 10;
        sg.elements.push_back(ec);
      }
      {
        EleChance ec;
        ec.weight = 8;
        ec.eleID = 16;
        sg.elements.push_back(ec);
      }
      SlotMachineConfig cfg;
      SlotMachine1 game(cfg);
      ast_eq(10, game.locateElement(0, forbid, sg));
      ast_eq(10, game.locateElement(3, forbid, sg));
      ast_eq(16, game.locateElement(5, forbid, sg));
      ast_eq(16, game.locateElement(12, forbid, sg));
      ast_eq(10, game.locateElement(13, forbid, sg));
    }

    void test_locateElement_has_forbid()
    {
      std::set<int32_t> forbid;
      forbid.insert(10);
      SlotGrid sg;
      {
        EleChance ec;
        ec.weight = 5;
        ec.eleID = 10;
        sg.elements.push_back(ec);
      }
      {
        EleChance ec;
        ec.weight = 8;
        ec.eleID = 16;
        sg.elements.push_back(ec);
      }
      SlotMachineConfig cfg;
      SlotMachine1 game(cfg);
      ast_eq(16, game.locateElement(0, forbid, sg));
      ast_eq(16, game.locateElement(3, forbid, sg));
      ast_eq(16, game.locateElement(5, forbid, sg));
      ast_eq(16, game.locateElement(12, forbid, sg));
      ast_eq(16, game.locateElement(13, forbid, sg));
    }

    void test_locateElement_no_ele()
    {
      std::set<int32_t> forbid;
      forbid.insert(10);
      forbid.insert(16);
      SlotGrid sg;
      {
        EleChance ec;
        ec.weight = 5;
        ec.eleID = 10;
        sg.elements.push_back(ec);
      }
      {
        EleChance ec;
        ec.weight = 8;
        ec.eleID = 16;
        sg.elements.push_back(ec);
      }
      SlotMachineConfig cfg;
      SlotMachine1 game(cfg);
      ast_eq(-1, game.locateElement(0, forbid, sg));
      ast_eq(-1, game.locateElement(3, forbid, sg));
      ast_eq(-1, game.locateElement(5, forbid, sg));
      ast_eq(-1, game.locateElement(12, forbid, sg));
      ast_eq(-1, game.locateElement(13, forbid, sg));
    }
private:
    bool _inited;
};
