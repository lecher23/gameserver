#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#define private public
#include <slots/slotmachine1.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
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

    void test_newLine_line_not_exist()
    {
      SlotMachineConfig cfg;
      SlotMachine1 game(cfg);
      game.newLine(2);
      ast_eq(1, game._lineInfo.size());
      ast_eq(1, game._lineInfo[2]);
    }

    void test_newLine_lien_exist()
    {
      SlotMachineConfig cfg;
      SlotMachine1 game(cfg);
      game._lineInfo[2] = 2;
      game.newLine(2);
      ast_eq(1, game._lineInfo.size());
      ast_eq(3, game._lineInfo[2]);
    }

    void test_countLines () {
      SlotMachineConfig cfg;
      std::vector<int32_t> line1 = {0,1,2,3};
      std::vector<int32_t> line2 = {8,5,2,7};
      cfg.linesConfig.push_back(line1);
      cfg.linesConfig.push_back(line2);
      SlotMachine1 game(cfg);
      std::vector<int32_t> result = {6, 5, 4, 3, 3, 4, 3, 4, 4, 3, 1, 3};
      game._result.swap(result);
      ast_eq(12, game._result.size());
      game.countLines();

      ast_eq(1, game._lineInfo.size());
      ast_eq(1, game._lineInfo[4]);
    }
private:
    bool _inited;
};
