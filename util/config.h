#ifndef UTIL_CONFIG_H
#define UTIL_CONFIG_H

#include "configure.h"
#include <string>

namespace cgserver{
    class Config{
    public:
	~Config();
	static Config &getInstance();
	bool initConfig(const std::string &configFilePath);
	bool refreshConfig();
	int getListenPort();
	bool getIntValue(const std::string &grp, const std::string &key, int &out);
	std::string getConfigValue(const std::string &grp, const std::string &key);
    private:
	Config();
	Config(Config &cfg);
	Config & operator = (const Config &);
	config_t *_cfg;
	std::string _configFilePath;
	
	static const std::string EmptyStr;
    };

}
#endif
