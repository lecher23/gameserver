#include "config.h"
#include <stdlib.h>

namespace cgserver{
    Config::~Config() {
	destroyConfig(_cfg);
    }

    Config::Config():_cfg(NULL){}

    Config &Config::getInstance() {
	static Config config;
	return config;
    }

#define GET_REQUIRED_STRING_VALUE(grp, key, value)                      \
    {                                                                   \
        std::string tmp = getConfigValue(grp, key);                     \
        if (tmp.empty()) {                                              \
	    CLOG(ERROR) << "Value ["<< grp << "." << key << "] not set."; \
	    return false;                                               \
        }                                                               \
        value = tmp;                                                \
    }


#define GET_REQUIRED_INT_VALUE(grp, key, value)                         \
    {                                                                   \
    	std::string intValue = getConfigValue(grp, key);                \
	if (intValue.empty()) {                                         \
	    CLOG(ERROR) << "Value ["<< grp << "." << key << "] not set."; \
	    return false;                                               \
	}                                                               \
        for (auto ch: intValue) {                                       \
	    if (ch < '0' || ch > '9') {                                 \
		CLOG(ERROR) << "Invalid value ["<< grp << "."           \
                            << key <<"]=[" << intValue << "]";          \
		return false;                                           \
	    }                                                           \
        }                                                               \
	value = atoi(intValue.c_str());                                 \
    }

#define GET_REQUIRED_BOOL_VALUE(grp, key, value)                        \
    {                                                                   \
        std::string tmpStr = getConfigValue(grp, key);                  \
        if (tmpStr == "0") {                                            \
            value = false;                                              \
        } else if (tmpStr == "1") {                                     \
            value = true;                                               \
        } else {                                                        \
            CLOG(ERROR) << "Invalid value in:" << key << " , expected:[0|1]"; \
            return false;                                               \
        }                                                               \
    }

    bool Config::initConfig(const std::string &configFilePath) {
        const std::string sGrpServer = "server";
        const std::string sGrpLua = "lua";
        const std::string sGrpDB = "db";
        const std::string sGrpRedis = "redis";
        const std::string sGrpSlot = "slot";
        const std::string sHost = "host";
        const std::string sThreadPoolSize = "thread_pool_size";
        const std::string sPort = "port";
        const std::string sUser = "user_name";
        const std::string sPassword = "user_pw";
        const std::string sDBName = "db_name";
        const std::string sLogDir = "log_dir";
        const std::string sLogLevel = "log_level";
        const std::string sNeedCheck = "need_check";
        const std::string sSecret = "secret";
        const std::string sPath = "path";
        const std::string sEnable = "enable";
        const std::string sUseAsio = "use_asio";
        const std::string sButterfly = "butterfly_cfg";
        const std::string sLevelCfg = "level_cfg";
        const std::string sLoginCfg = "login_cfg";
        const std::string sCjCfg = "achievement_cfg";
        const std::string sVipCfg = "vip_cfg";

	config_t *tmp = parseConfigFile(configFilePath.c_str());
	// only parse config success can change _cfg
	if (tmp == nullptr) {
            return false;
        }
        // free previous config
        destroyConfig(_cfg);
        //displayConfig(_cfg);
        _cfg = tmp;
        _configFilePath = configFilePath;

        GET_REQUIRED_INT_VALUE(sGrpServer, sPort, server_port);
        GET_REQUIRED_INT_VALUE(sGrpServer, sThreadPoolSize, server_thread_pool);
        log_dir = getConfigValue(sGrpServer, sLogDir);
        std::string tmpStr = getConfigValue(sGrpServer, sLogLevel);
        log_level = 0;
        if (tmpStr == "WARNNIG") {
            log_level = 1;
        } else if (tmpStr == "ERROR") {
            log_level = 2;
        } else if (tmpStr == "FATAL") {
            log_level = 3;
        }
        net_secret = getConfigValue(sGrpServer, sSecret);
        GET_REQUIRED_BOOL_VALUE(sGrpServer, sNeedCheck, need_check);

        GET_REQUIRED_BOOL_VALUE(sGrpLua, sEnable, lua_enable);
        if (lua_enable) {
            GET_REQUIRED_STRING_VALUE(sGrpLua, sPath, lua_path);
        }

        GET_REQUIRED_STRING_VALUE(sGrpDB, sHost, db_host);
        GET_REQUIRED_INT_VALUE(sGrpDB, sPort, db_port);
        GET_REQUIRED_STRING_VALUE(sGrpDB, sUser, db_user);
        GET_REQUIRED_STRING_VALUE(sGrpDB, sPassword, db_pw);
        GET_REQUIRED_STRING_VALUE(sGrpDB, sDBName, db_name);
        GET_REQUIRED_INT_VALUE(sGrpDB, sThreadPoolSize, db_pool_size);

        GET_REQUIRED_STRING_VALUE(sGrpRedis, sHost, redis_host);
        GET_REQUIRED_INT_VALUE(sGrpRedis, sPort, redis_port);

        GET_REQUIRED_STRING_VALUE(sGrpSlot, sButterfly, butterfly_path);
        GET_REQUIRED_STRING_VALUE(sGrpSlot, sLevelCfg, level_cfg_path);
        GET_REQUIRED_STRING_VALUE(sGrpSlot, sLoginCfg, login_cfg_path);
        GET_REQUIRED_STRING_VALUE(sGrpSlot, sCjCfg, cj_cfg_path);
        GET_REQUIRED_STRING_VALUE(sGrpSlot, sVipCfg, vip_cfg_path);

        return true;
    }

    bool Config::refreshConfig() {
	if (!initConfig(_configFilePath)) {
	    CLOG(ERROR) << "Refresh config failed. Will not change previous config.";
	    return false;
	}
	return true;
    }

    std::string Config::getConfigValue(const std::string &grp, const std::string &key){
        static const std::string EmptyStr;
	config_group_t *cfgGrp = findConfigGroup(_cfg, grp.c_str());
	if (cfgGrp == NULL) {
	    return EmptyStr;
	}
	const char *v = cgserver::getConfigValue(cfgGrp, key.c_str());
	return std::string(v);
    }

#undef GET_REQUIRED_STRING_VALUE
#undef GET_REQUIRED_INT_VALUE
#undef GET_REQUIRED_BOOL_VALUE

}

