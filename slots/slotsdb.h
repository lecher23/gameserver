#ifndef SLOTSDB_H
#define SLOTSDB_H

#include <stdint.h>
#include <string>
#include "../mysql/mysqlclient.h"

namespace slots{
    struct UserInfo{
	std::string uid;
	std::string mid;
	std::string fname;
	//std::string lname;
	std::string avatar;
	std::string male; // 0 or 1
	std::string country;
	/* std::string level; */
	/* std::string exp; */
	/* std::string fortune; */
	/* std::string vip_level; */
    };

    struct UserResource {
	std::string uid;
	std::string level;
	std::string exp;
	std::string fortune;
	std::string vipLevel;
    };

    struct SlotsUser{
	UserInfo uInfo;
	UserResource uRes;
    };

    DF_SHARED_PTR(SlotsUser);
    
    class SlotsDB{
    public:
        ~SlotsDB();

	bool getUserInfo(const std::string &mid, SlotsUser &su, std::string &errMsg) const ;
	bool addUser(const std::string &mid, std::string &uid, std::string &errMsg)const ;
	/* Only update table user_info. */
	bool updateUserInfo(const UserInfo &ui, std::string &errMsg) const;

	static SlotsDB &getInstance();
	
    private:
        SlotsDB();
	SlotsDB(SlotsDB &);
	cgserver::MysqlClient &_client;
    };
}
#endif
