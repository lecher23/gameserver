#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <util/config.h>
#include "/home/licheng/workspace/CgServerPlus/test/common.h"

#define ast_eq(x, y) TS_ASSERT_EQUALS(x, y);
#define ast_true(x) TS_ASSERT(x);

using namespace cgserver;
// using namespace slots;

const std::string lua_file = cxx::lua_file;
const std::string cfg_file = cxx::cfg_file;

class testConfig: public CxxTest::TestSuite 
{
public:
    testConfig():_inited(false){}

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_init_config()
    {
        auto &cfg = Config::getInstance();
        ast_true(Config::getInstance().initConfig("config_files/ConfigTest.cfg"));

        ast_eq(9877, cfg.getServerPort());
        ast_eq(8, cfg.getServerThreadPoolSize());
        ast_eq("logs", cfg.getLogDir());
        ast_eq(0, cfg.getLogLevel());
        ast_eq("2x901as", cfg.getNetSecret());
        ast_true(!cfg.needCheckNet());

        ast_true(!cfg.enableLua());

        ast_eq("localhost", cfg.getDBHost());
        ast_eq("root", cfg.getDBUserName());
        ast_eq("111222", cfg.getDBPassword());
        ast_eq("SLOTS", cfg.getDBName());
        ast_eq(3307, cfg.getDBPort());
        ast_eq(4, cfg.getDBConnPoolSize());

        ast_true(cfg.enableRedis());
        ast_eq("127.0.0.2", cfg.getRedisHost());
        ast_eq(6378, cfg.getRedisPort());

        ast_eq("slot_config/butterfly.json", cfg.getButterflyConfigPath());
        ast_eq("slot_config/Level.json", cfg.getLevelConfigPath());
        ast_eq("slot_config/LoginReward.json", cfg.getLoginConfigPath());
        ast_eq("slot_config/Achievement.json", cfg.getAchievementConfigPath());
        ast_eq("slot_config/VIPConfig.json", cfg.getVipConfigPath());
    }
private:
    bool _inited;
};
