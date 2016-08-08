#include "config.h"
#include <stdlib.h>

namespace cgserver{
    const std::string Config::EmptyStr = "";
    
    Config::~Config() {
	destroyConfig(_cfg);
    }

    Config::Config():_cfg(NULL){}

    Config &Config::getInstance() {
	static Config config;
	return config;
    }

    bool Config::initConfig(const std::string &configFilePath) {
	config_t *tmp = parseConfigFile(configFilePath.c_str());
	// only parse config success can change _cfg
	if (tmp != NULL) {
	    // free previous config
	    destroyConfig(_cfg);
	    _cfg = tmp;
	    //displayConfig(_cfg);
	    _configFilePath = configFilePath;
	    return true;
	}
	return false;
    }

    bool Config::refreshConfig() {
	if (!initConfig(_configFilePath)) {
	    CLOG(ERROR) << "Refresh config failed. Will not change previous config.";
	    return false;
	}
	return true;
    }

    std::string Config::getConfigValue(const std::string &grp, const std::string &key){
	config_group_t *cfgGrp = findConfigGroup(_cfg, grp.c_str());
	if (cfgGrp == NULL) {
	    return EmptyStr;
	}
	const char *v = cgserver::getConfigValue(cfgGrp, key.c_str());
	return std::string(v);
    }

    int Config::getListenPort(){
	int i_port;
	if (!Config::getIntValue("server", "port", i_port)) {
	    return -1;
	}
	return i_port;
    }

    bool Config::getIntValue(const std::string &grp, const std::string &key, int &out) {
	std::string intValue = getConfigValue(grp, key);
	if (intValue.size() == 0) {
	    CLOG(ERROR) << "Value ["<< grp << "." << key << "] not set.";
	    return false;
	}
	for (int i = 0; i < intValue.size(); ++i) {
	    if (intValue[i] < '0' || intValue[i] > '9') {
		CLOG(ERROR) << "Invalid value ["<< grp << "." << key <<"]=[" << intValue << "]";
		return false;
	    }
	}
	out = atoi(intValue.c_str());
	return true;
    }
}
