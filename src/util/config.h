#ifndef UTIL_CONFIG_H
#define UTIL_CONFIG_H

#include "configure.h"
#include <string>

namespace cgserver{
    #define MAX_POOL_SIZE 12
    class Config{
    public:
	~Config();
	static Config &getInstance();
	bool initConfig(const std::string &configFilePath);
	bool refreshConfig();

        int32_t getServerPort() {return server_port;}
        int32_t getServerThreadPoolSize() {return server_thread_pool;}
        int32_t getLogLevel() {return log_level;}

        std::string getLogDir() {return log_dir;}
        std::string getNetSecret() {return net_secret;}
        bool needCheckNet() {return need_check;}
        bool useAsio() {return use_asio;}

        bool enableLua() {return lua_enable;}
        std::string getLuaPath() {return lua_path;}

        std::string getDBHost() {return db_host;}
        std::string getDBUserName() {return db_user;}
        std::string getDBPassword() {return db_pw;}
        std::string getDBName() {return db_name;}
        int32_t getDBPort() {return db_port;}
        int32_t getDBConnPoolSize() {return db_pool_size;}

        bool enableRedis() {return redis_enable;}
        std::string getRedisHost() {return redis_host;}
        int32_t getRedisPort() {return redis_port;}

        int32_t getMaxFriendsLimit() {return max_friends;}
        int32_t getSendGiftTimesLimit() {return daily_max_gift;}
        std::string getButterflyConfigPath() {return butterfly_path;}
        std::string getLevelConfigPath() {return level_cfg_path;}
        std::string getLoginConfigPath() {return login_cfg_path;}
        std::string getAchievementConfigPath() {return cj_cfg_path;}
        std::string getVipConfigPath() {return vip_cfg_path;}

    private:
	Config();
	Config(Config &cfg);
	Config & operator = (const Config &);
	config_t *_cfg;
        std::string _configFilePath;
	std::string getConfigValue(const std::string &grp, const std::string &key);

        int32_t server_port;
        int32_t server_thread_pool;
        std::string log_dir;
        std::string net_secret;
        int32_t log_level{0};
        bool need_check;
        bool use_asio{true};

        bool lua_enable;
        std::string lua_path;

        std::string db_host{"127.0.0.1"};
        int32_t db_port{3306};
        int32_t db_pool_size{4};
        std::string db_user;
        std::string db_pw;
        std::string db_name;

        bool redis_enable{true};
        std::string redis_host{"127.0.0.1"};
        int32_t redis_port{6379};

        int32_t max_friends{50};
        int32_t daily_max_gift{50};
        std::string butterfly_path;
        std::string level_cfg_path;
        std::string login_cfg_path;
        std::string cj_cfg_path;
        std::string vip_cfg_path;
    };

}
#endif
