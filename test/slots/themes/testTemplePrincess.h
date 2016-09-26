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
      for (int i = 1; i < 8; ++i) {
        cfg.addElement(i, NORMAL_ELEMENT);
      }
      std::vector<int32_t> line1 = {0,1,2,3};
      std::vector<int32_t> line2 = {8,5,2,7};
      std::vector<int32_t> line3 = {8,5,10,11};
      cfg.addLine(21, line1);
      cfg.addLine(38, line2);
      cfg.addLine(66, line3);
      cfg.setColumnNumber(4);
      TemplePrincess game(cfg);
      std::map<int32_t, int32_t> result =
              {{0,6}, {1,1}, {2,4}, {3,3},
               {4,3}, {5,4}, {6,3}, {7,1},
               {8,4}, {9,1}, {10,3}, {11,2}};
      GameResult grd;
      grd.gridsData.swap(result);
      ast_eq(12, grd.gridsData.size());
      game.countLines(grd);
      ast_eq(2, grd.lines.size());
      ast_eq(3, grd.lines[0].count);
      ast_eq(4, grd.lines[0].eleID);
      ast_eq(38, grd.lines[0].lineID);
      ast_eq(2, grd.lines[1].count);
      ast_eq(4, grd.lines[1].eleID);
      ast_eq(66, grd.lines[1].lineID);
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
    ast_eq(cnt, grd.lines[0].count);         \
    ast_eq(id, grd.lines[0].eleID);            \
    ast_eq(21, grd.lines[0].lineID);            \
    grd.lines.clear();

    void test_countLines_with_wild() {
      // normal element: 1, 2, 3
      // wild element: 4, 5
      // other element:6, 7
      TSConfig cfg;
      cfg.addElement(0, NORMAL_ELEMENT);
      cfg.addElement(1, NORMAL_ELEMENT);
      cfg.addElement(2, NORMAL_ELEMENT);
      cfg.addElement(3, NORMAL_ELEMENT);

      cfg.addElement(4, WILD_ELEMENT);
      cfg.addElement(5, WILD_ELEMENT);
      cfg.addElement(6, WILD_ELEMENT);

      cfg.addElement(7, (WILD_ELEMENT + NORMAL_ELEMENT) * 2);

      std::vector<int32_t> line1 = {0,1,2,3,4};
      cfg.addLine(21, line1);
      cfg.setColumnNumber(5);
      TemplePrincess game(cfg);
      GameResult grd;

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
      cfg.addElement(0, NORMAL_ELEMENT);
      cfg.addElement(1, NORMAL_ELEMENT);
      cfg.addElement(2, NORMAL_ELEMENT);
      cfg.addElement(3, NORMAL_ELEMENT);

      cfg.addElement(4, WILD_ELEMENT);
      cfg.addElement(5, WILD_ELEMENT);
      cfg.addElement(6, WILD_ELEMENT);

      cfg.addElement(7, (WILD_ELEMENT + NORMAL_ELEMENT) * 2);
      GameResult grd;

      std::vector<int32_t> line1 = {0,1,2,3,4};
      cfg.addLine(21, line1);
      cfg.setColumnNumber(5);
      TemplePrincess game(cfg);
      std::map<int32_t, int32_t> result7 =
          {{0,W1}, {1,W1}, {2,W1}, {3,W1},{4,W1}};
      ASSERT_ZERO_RESULT(result7);
    }

    void test_locateElement_no_forbid()
    {
      std::set<int32_t> forbid;
      TSGrid sg;
      sg.addElement(10, 5);
      sg.addElement(16, 8);
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
      sg.addElement(10, 5);
      sg.addElement(16, 8);
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
      sg.addElement(10, 5);
      sg.addElement(16, 8);
      TSConfig cfg;
      TemplePrincess game(cfg);
      ast_eq(-1, game.locateElement(0, forbid, sg));
      ast_eq(-1, game.locateElement(3, forbid, sg));
      ast_eq(-1, game.locateElement(5, forbid, sg));
      ast_eq(-1, game.locateElement(12, forbid, sg));
      ast_eq(-1, game.locateElement(13, forbid, sg));
    }

    void test_play()
    {
      ast_true(Config::getInstance().initConfig(cfg_file));
      ast_true(MysqlConnPool::getInstance().init());
      ast_true(SlotsConfig::getInstance().init());
      TemplePrincess game(SlotsConfig::getInstance().themeConfig.tsConfig);
      int32_t totalEarned = 0;
      int32_t totalCost = 0;
      std::map<int32_t, int32_t> ele;
      for (int i = 0; i < 100; ++i) {
          GameResult data;
          data.bet = 100;
          data.lineNumber = 10;
          if (!game.play(data)) {
            CLOG(ERROR) << "play meet error.";
            break;
          }
          ast_true(data.gridsData.size() > 0);
          for (auto &item: data.gridsData) {
            if(item.second == 0) {
              CLOG(ERROR) << "catch 0 on position:" << item.first;
              auto y = 100/item.second;
            }
            auto itr = ele.find(item.second);
            if(itr == ele.end()) {
              ele[item.second] = 0;
            }
            ele[item.second] ++;
          }
          totalEarned += data.earned;
          totalCost += data.bet * data.lineNumber;
      }
      CLOG(INFO) << "total earned:" << totalEarned;
      CLOG(INFO) << "total cost  :" << totalCost;
      for (auto &item: ele){
        CLOG(INFO) << "ele :" << item.first << ", count:" << item.second;
      }
    }
private:
    bool _inited;
};
