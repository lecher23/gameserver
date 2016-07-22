#include "config.h"
#include <stdlib.h>

namespace cgserver{
    const std::string Config::ConfigFilePath = "server.cfg";
    const std::string Config::EmptyStr = "";
    
    Config::~Config() {
	destroyConfig(_cfg);
    }

    Config::Config():_cfg(NULL){}

    Config &Config::getInstance() {
	static Config config;
	return config;
    }

    bool Config::initConfig() {
	config_t *tmp = parseConfigFile(ConfigFilePath.c_str());
	// only parse config success can change _cfg
	if (tmp != NULL) {
	    // free previous config
	    destroyConfig(_cfg);
	    _cfg = tmp;
	    displayConfig(_cfg);	    
	    return true;
	}
	return false;
    }

    bool Config::refreshConfig() {
	if (!initConfig()) {
	    std::cout << "Refresh config failed. Will not change previous config." << std::endl;
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
	std::string s_port = getConfigValue("server", "port");
	if (s_port.size() == 0) {
	    std::cout << "Port not set." << std::endl;
	    return -1;
	}
	for (int i = 0; i < s_port.size(); ++i) {
	    if (s_port[i] < '0' || s_port[i] > '9') {
		std::cout << "Invalid port." << std::endl;
		return -1;
	    }
	}
	return atoi(s_port.c_str());
    }
}
