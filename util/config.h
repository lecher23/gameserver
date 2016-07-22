#ifndef UTIL_CONFIG_H
#define UTIL_CONFIG_H

#include "configure.h"
#include <string>

namespace cgserver{
    class Config{
    public:
	~Config();
	static Config &getInstance();
	bool initConfig();
	bool refreshConfig();
	int getListenPort();
	std::string getConfigValue(const std::string &grp, const std::string &key);
    private:
	Config();
	Config & operator = (const Config &);
	config_t *_cfg;
	static const std::string ConfigFilePath;
	static const std::string EmptyStr;
    };

}
#endif
