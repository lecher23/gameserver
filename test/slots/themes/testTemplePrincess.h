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
    testTemplePrincess():_inited(false){
	if (!_inited){
	    ast_true(Config::getInstance().initConfig(cfg_file));
	    _inited = true;
	}
    }

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
        cfg.addElement(i, NORMAL_ELEMENT, false);
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

    void init_tsconfig(TSConfig &cfg) {
      cfg.addElement(0, NORMAL_ELEMENT, false);
      cfg.addElement(1, NORMAL_ELEMENT, false);
      cfg.addElement(2, NORMAL_ELEMENT, false);
      cfg.addElement(3, NORMAL_ELEMENT, false);

      cfg.addElement(4, WILD_ELEMENT, false);
      cfg.addElement(5, WILD_ELEMENT, false);
      cfg.addElement(6, WILD_ELEMENT, false);

      cfg.addElement(7, (WILD_ELEMENT + NORMAL_ELEMENT) * 2, false);
    }

    void test_countLines_with_wild() {
      // normal element: 1, 2, 3
      // wild element: 4, 5
      // other element:6, 7
      TSConfig cfg;
      init_tsconfig(cfg);

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
      init_tsconfig(cfg);
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
      sg.addElement(1, 3);
      sg.addElement(2, 1);
      sg.addElement(3, 1);
      sg.addElement(4, 7);
      sg.addElement(5, 6);
      sg.addElement(6, 6);
      sg.addElement(7, 6);
      sg.addElement(8, 2);
      sg.addElement(9, 2);
      sg.addElement(10, 2);
      sg.addElement(11, 1);
      sg.addElement(12, 1);
      CLOG(INFO) << "Total weight:" << sg.getTotalWeight();
      TSConfig cfg;
      TemplePrincess game(cfg);
      ast_eq(1, game.locateElement(0, forbid, sg));
      ast_eq(1, game.locateElement(1, forbid, sg));
      ast_eq(1, game.locateElement(2, forbid, sg));
      ast_eq(2, game.locateElement(3, forbid, sg));
      ast_eq(3, game.locateElement(4, forbid, sg));
      ast_eq(11, game.locateElement(36, forbid, sg));
      ast_eq(12, game.locateElement(37, forbid, sg));
      ast_eq(1, game.locateElement(38, forbid, sg));
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

    void append_dict(std::map<int32_t, std::map<int32_t, int32_t>> &dict,
                     int32_t index, int32_t eleID)
    {
        auto itr1 = dict[index].find(eleID);
        if (itr1 == dict[index].end()) {
            dict[index][eleID] = 0;
        }
        ++dict[index][eleID];
    }

    void print_dict(std::map<int32_t, std::map<int32_t, int32_t>> &dict) {
        for (auto &grid: dict) {
            printf("-------INDEX[%d]-------\n", grid.first);
            for (auto &ele: grid.second) {
                printf("%5d, %5d\n", ele.first, ele.second);
            }
        }
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
      std::map<int32_t, std::map<int32_t, int32_t>> gridInfo;
      for (int i = 0; i < 1000; ++i) {
          GameResult data;
          data.bet = 1000;
          data.lineNumber = 10;
          if (!game.play(data)) {
            CLOG(ERROR) << "play meet error.";
            break;
          }
          ast_true(data.gridsData.size() > 0);
          for (auto &item: data.gridsData) {
              append_dict(gridInfo, item.first, item.second);
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
          totalEarned += data.earned.normal;
          totalCost += data.bet;
      }
      print_dict(gridInfo);
      CLOG(INFO) << "total earned:" << totalEarned;
      CLOG(INFO) << "total cost  :" << totalCost;
      for (auto &item: ele){
        CLOG(INFO) << "ele :" << item.first << ", count:" << item.second;
      }
    }
private:
    bool _inited;
};
