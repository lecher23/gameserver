#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"
#define private public
#include <slots/processors/gameresultprocessor.h>
#undef private
#include <slots/slotsconfig.h>

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

// using namespace cgserver;
using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testGameResultProcessor: public CxxTest::TestSuite 
{
public:
testGameResultProcessor():_inited(false){
    if (!_inited) {
      initSlotsConfig();
      _inited = true;
    }
  }

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void initSlotsConfig() {
      auto &cfg = SlotsConfig::getInstance();
      cfg.bet2Exp.addConfig(100, 50);
      cfg.bet2Exp.addConfig(200, 150);
      cfg.bet2Exp.addConfig(300, 200);
      cfg.bet2Exp.addConfig(1000, 800);
      cfg.levelConfig[1].level = 1;
      cfg.levelConfig[1].expNeed = 100;
      cfg.levelConfig[2].level = 2;
      cfg.levelConfig[2].expNeed = 300;
      cfg.levelConfig[3].level = 3;
      cfg.levelConfig[3].expNeed = 600;
      cfg.levelConfig[4].level = 4;
      cfg.levelConfig[4].expNeed = 900;
    }

    void setUserResource(UserResource &uRes) {
      uRes.level = 1;
      uRes.exp = 1;
      uRes.fortune = 1000;
      uRes.vipLevel = 1;
      uRes.vipPoint = 10;
    }

    void setUserHistory(UserHistory &uHis) {
      uHis.maxFortune = 99;
      uHis.maxEarned = 99;
      uHis.totalEarned = 10;
      uHis.totalBet = 10;
      uHis.changed = false;
    }

#define INIT_GAMECONTEXT_GAMERESULT(mEarned, mBet, freegame)    \
    GameContext gc;                                             \
    auto su = std::make_shared<SlotsUser>();                    \
    setUserResource(su->uRes);                                  \
    setUserHistory(su->uHis);                                   \
    gc.user = su;                                               \
    GameResult data;                                            \
    data.earned = mEarned;                                      \
    data.bet = mBet;                                            \
    data.bFreeGame = freegame;

    void test_processMoney_balance( void )
    {
      INIT_GAMECONTEXT_GAMERESULT(1000, 1000, false);
      GameResultProcessor grp;
      grp.processMoney(gc, data);
      ast_eq(1000, su->uRes.fortune);
    }

    void test_processMoney_lost_money( void )
    {
      INIT_GAMECONTEXT_GAMERESULT(100, 1000, false);
      GameResultProcessor grp;
      grp.processMoney(gc, data);
      ast_eq(100, su->uRes.fortune);
      ast_eq(100, su->uHis.maxEarned);
      ast_eq(100, su->uHis.maxFortune);
      ast_eq(1010, su->uHis.totalBet);
      ast_eq(110, su->uHis.totalEarned);
      ast_true(su->uHis.changed);
    }

    void test_processMoney_earned_money( void )
    {
      INIT_GAMECONTEXT_GAMERESULT(3000, 1000, false);
      GameResultProcessor grp;
      grp.processMoney(gc, data);

      ast_eq(3000, su->uRes.fortune);
      ast_eq(3000, su->uHis.maxEarned);
      ast_eq(3000, su->uHis.maxFortune);
      ast_eq(1010, su->uHis.totalBet);
      ast_eq(3010, su->uHis.totalEarned);
      ast_true(su->uHis.changed);
    }

    void test_processExp_level_not_up()
    {
      INIT_GAMECONTEXT_GAMERESULT(0, 100, false);
      GameResultProcessor grp;
      grp.processExp(gc, data);

      ast_eq(1, su->uRes.level);
      ast_eq(51, su->uRes.exp);
    }


    void test_processExp_free_game()
    {
      INIT_GAMECONTEXT_GAMERESULT(10000, 100, true);
      GameResultProcessor grp;
      grp.processExp(gc, data);

      ast_eq(1, su->uRes.level);
      ast_eq(1, su->uRes.exp);
    }

    void test_processExp_levelup_one_time()
    {
      INIT_GAMECONTEXT_GAMERESULT(10000, 200, false);
      GameResultProcessor grp;
      grp.processExp(gc, data);

      ast_eq(2, su->uRes.level);
      ast_eq(151, su->uRes.exp);
    }

    void test_processExp_levelup_multi_times()
    {
      INIT_GAMECONTEXT_GAMERESULT(10000, 1000, false);
      GameResultProcessor grp;
      grp.processExp(gc, data);

      ast_eq(4, su->uRes.level);
      ast_eq(801, su->uRes.exp);
      ast_eq(3, gc.events.size());
    }
private:
    bool _inited;
};
