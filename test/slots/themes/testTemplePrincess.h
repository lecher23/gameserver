#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#include <mysql/mysqlconnpool.h>
#include <slots/slotsconfig.h>
#include <util/config.h>
#define private public
#include <slots/themes/templeprincess.h>
#undef private

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testTemplePrincess: public CxxTest::TestSuite 
{
public:
    testTemplePrincess():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_countLines () {
      // normal element: 1, 2, 3
      // wild element: 4, 5
      // other element:6, 7
      TSConfig cfg;
      cfg.elements[1].type = 0;
      cfg.elements[2].type = 0;
      cfg.elements[3].type = 0;
      cfg.elements[4].type = 0;
      cfg.elements[5].type = 0;
      cfg.elements[6].type = 0;
      cfg.elements[7].type = 0;
      std::vector<int32_t> line1 = {0,1,2,3};
      std::vector<int32_t> line2 = {8,5,2,7};
      std::vector<int32_t> line3 = {8,5,10,11};
      cfg.lines[21].swap(line1);
      cfg.lines[38].swap(line2);
      cfg.lines[66].swap(line3);
      cfg.maxColumn = 4;
      TemplePrincess game(cfg);
      std::map<int32_t, int32_t> result =
              {{0,6}, {1,1}, {2,4}, {3,3},
               {4,3}, {5,4}, {6,3}, {7,1},
               {8,4}, {9,1}, {10,3}, {11,2}};
      GameResultData grd;
      grd.gridsData.swap(result);
      ast_eq(12, grd.gridsData.size());
      game.countLines(grd);
      ast_eq(2, grd.lines.size());
      ast_eq(3, grd.lines[38].count);
      ast_eq(4, grd.lines[38].eleID);
      ast_eq(2, grd.lines[66].count);
      ast_eq(4, grd.lines[66].eleID);
    }
#define N0 0
#define N1 1
#define W1 5
#define W2 4
#define O 7
#define O3 7
#define ASSERT_ZERO_RESULT(input)               \
    grd.gridsData.swap(input);                \
    ast_eq(5, grd.gridsData.size());            \
    game.countLines(grd);                       \
    ast_eq(0, grd.lines.size());                \
    grd.lines.clear();

#define ASSERT_ONE_RESULT(input, cnt, id)     \
    grd.gridsData.swap(input);                  \
    ast_eq(5, grd.gridsData.size());            \
    game.countLines(grd);                       \
    ast_eq(1, grd.lines.size());                \
    ast_eq(cnt, grd.lines[21].count);         \
    ast_eq(id, grd.lines[21].eleID);            \
    grd.lines.clear();

    void test_countLines_with_wild() {
      // normal element: 1, 2, 3
      // wild element: 4, 5
      // other element:6, 7
      TSConfig cfg;
      cfg.elements[0].type = NORMAL_ELEMENT;
      cfg.elements[1].type = NORMAL_ELEMENT;
      cfg.elements[2].type = NORMAL_ELEMENT;
      cfg.elements[3].type = NORMAL_ELEMENT;

      cfg.elements[4].type = WILD_ELEMENT;
      cfg.elements[5].type = WILD_ELEMENT;
      cfg.elements[6].type = WILD_ELEMENT;

      cfg.elements[7].type = (WILD_ELEMENT + NORMAL_ELEMENT) * 2;

      std::vector<int32_t> line1 = {0,1,2,3,4};
      cfg.lines[21].swap(line1);
      cfg.maxColumn = 5;
      TemplePrincess game(cfg);
      GameResultData grd;

      // start with NORMAL
      std::map<int32_t, int32_t> result1 =
          {{0,N0}, {1,N1}, {2,W2}, {3,N1},{4,O}};
      std::map<int32_t, int32_t> result2 =
          {{0,N0}, {1,W2}, {2,N0}, {3,W2},{4,O}};
      std::map<int32_t, int32_t> result3 =
          {{0,N0}, {1,W2}, {2,N1}, {3,W2},{4,O}};
      std::map<int32_t, int32_t> result4 =
          {{0,N0}, {1,W2}, {2,W2}, {3,N1},{4,O}};

      ASSERT_ZERO_RESULT(result1);

      ASSERT_ONE_RESULT(result2, 4, N0);
      ASSERT_ONE_RESULT(result3, 2, N0);
      ASSERT_ONE_RESULT(result4, 3, N0);

      // start with wild
      std::map<int32_t, int32_t> result5 =
          {{0,W1}, {1,N0}, {2,N1}, {3,N0},{4,O}};
      std::map<int32_t, int32_t> result6 =
          {{0,W1}, {1,W1}, {2,N0}, {3,N0},{4,N1}};

      ASSERT_ONE_RESULT(result5, 2, N0);
      ASSERT_ONE_RESULT(result6, 4, N0);

      // start with other
      std::map<int32_t, int32_t> result8 =
          {{0,O3}, {1,W1}, {2,N0}, {3,N0},{4,N0}};
      std::map<int32_t, int32_t> result9 =
          {{0,O3}, {1,N0}, {2,N0}, {3,N0},{4,N0}};
      std::map<int32_t, int32_t> result10 =
          {{0,O3}, {1,O3}, {2,O3}, {3,O3},{4,O3}};

      ASSERT_ZERO_RESULT(result8);
      ASSERT_ZERO_RESULT(result9);
      ASSERT_ZERO_RESULT(result10);
    }

    void test_countLine_all_wild()
    {
      TSConfig cfg;
      cfg.elements[0].type = NORMAL_ELEMENT;
      cfg.elements[1].type = NORMAL_ELEMENT;
      cfg.elements[2].type = NORMAL_ELEMENT;
      cfg.elements[3].type = NORMAL_ELEMENT;

      cfg.elements[4].type = WILD_ELEMENT;
      cfg.elements[5].type = WILD_ELEMENT;
      cfg.elements[6].type = WILD_ELEMENT;

      cfg.elements[7].type = (WILD_ELEMENT + NORMAL_ELEMENT) * 2;
      GameResultData grd;

      std::vector<int32_t> line1 = {0,1,2,3,4};
      cfg.lines[21].swap(line1);
      cfg.maxColumn = 5;
      TemplePrincess game(cfg);
      std::map<int32_t, int32_t> result7 =
          {{0,W1}, {1,W1}, {2,W1}, {3,W1},{4,W1}};
      ASSERT_ZERO_RESULT(result7);
    }

    void test_play()
    {
      ast_true(Config::getInstance().initConfig(cfg_file));
      ast_true(MysqlConnPool::getInstance().init());
      ast_true(SlotsConfig::getInstance().init());
      TemplePrincess game(SlotsConfig::getInstance().themeConfig.tsConfig);
      GameResultData data;
      game.play(data);
      ast_true(data.gridsData.size() > 0);
      for (auto &item: data.gridsData) {
        CLOG(INFO) << item.first << "," << item.second;
      }
      for (auto &item: data.lines) {
        CLOG(INFO) <<"Line id:"<< item.first << ", Line count:"
                   << item.second.count << ", Ele id:" << item.second.eleID;
      }
      CLOG(INFO) << "test play() done.";
    }

    void test_locateElement_no_forbid()
    {
      std::set<int32_t> forbid;
      TSGrid sg;
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
      TSConfig cfg;
      TemplePrincess game(cfg);
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
      TSGrid sg;
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
      TSConfig cfg;
      TemplePrincess game(cfg);
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
      TSGrid sg;
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
      TSConfig cfg;
      TemplePrincess game(cfg);
      ast_eq(-1, game.locateElement(0, forbid, sg));
      ast_eq(-1, game.locateElement(3, forbid, sg));
      ast_eq(-1, game.locateElement(5, forbid, sg));
      ast_eq(-1, game.locateElement(12, forbid, sg));
      ast_eq(-1, game.locateElement(13, forbid, sg));
    }
private:
    bool _inited;
};
