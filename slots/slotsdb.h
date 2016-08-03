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
	std::string avatar;
	std::string male; // 0 or 1
	std::string country;
    };
    DF_SHARED_PTR(UserInfo);        

    struct UserResource {
	std::string uid;
	std::string level;
	std::string exp;
	std::string fortune;
	std::string vipLevel;
    };
    DF_SHARED_PTR(UserResource);    

    struct SlotsUser{
	UserInfo uInfo;
	UserResource uRes;
    };
    DF_SHARED_PTR(SlotsUser);    

    /*shared by other user.*/
    struct SlotsMail{
	std::string mailId;
	std::string title;
	std::string content;
	std::string attachment;
	std::string createTime;
	std::string keepDays;
    };
    DF_SHARED_PTR(SlotsMail);

    /*owned by one user*/
    struct UserMail{
	SlotsMail mailInfo;
	bool bRead;
	bool bGet;
	bool bDel;
    };
    DF_SHARED_PTR(UserMail);
    
    /* user mails*/
    typedef std::vector<UserMailPtr> UserMails;
    
    class SlotsDB{
    public:
        ~SlotsDB();

	bool getUserInfo(const std::string &mid, SlotsUser &su, std::string &errMsg) const ;
	bool addUser(const std::string &mid, std::string &uid, std::string &errMsg)const ;
	/* Only update table user_info. */
	bool updateUserInfo(const UserInfo &ui, std::string &errMsg) const;
	bool getUserMails(const MyString &uid, MyString &offset, MyString &count, UserMails &out);

	static SlotsDB &getInstance();
    private:
	bool getMailInfo(const cgserver::MysqlRows &mails, UserMails &out);
	
        SlotsDB();
	SlotsDB(SlotsDB &);
	cgserver::MysqlClient &_client;
    };
}
#endif
