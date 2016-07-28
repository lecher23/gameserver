#ifndef SLOTSDB_H
#define SLOTSDB_H

#include <stdint.h>
#include <string>

namespace slots{
    struct UserInfo{
	std::string uid;
	std::string mid;
	std::string uname;
	std::string level;
	std::string exp;
    UserInfo():uid(""),mid(""),uname(""),level(""),exp(""){}
    };
    
    class SlotsDB{
    public:
        SlotsDB();
        ~SlotsDB();

	bool getUserInfo(const std::string &mid, UserInfo &ui, std::string &errMsg);
	bool setUserInfo(const UserInfo &ui, std::string &errMsg);
    };
}
#endif
