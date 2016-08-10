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
	bool changed;
    };
    DF_SHARED_PTR(UserInfo);        

    struct UserResource {
	std::string uid;
	uint32_t level;
	uint64_t exp;
	int64_t fortune;
	uint32_t vipLevel;
	bool changed;
	
	void levelUp() {
	    level ++;
	    changed = true;
	}

	void vipLevelUp(){
	    level ++;
	    changed = true;
	}

	void mdfyFortune(int64_t earned) {
	    if (earned == 0)
		return;
	    fortune += earned;
	    if (fortune < 0) fortune = 0;
	    changed = true;
	}
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

    struct FHistory{
	std::string uid;
	int32_t inviteCount;
	int64_t totalReward;
	int64_t rewardRemain;
    };
    DF_SHARED_PTR(FHistory);    
    
    /* user mails*/
    typedef std::vector<UserMailPtr> UserMails;
    typedef std::vector<SlotsUserPtr> SlotsUsers;
    typedef SlotsUsers FriendsList;

    class SlotsDB{
    public:
        ~SlotsDB();

	bool getUserInfo(const std::string &query, SlotsUser &su) const ;
	bool getUserInfoByMachineId(const std::string &mid, SlotsUser &su) const ;
	bool getUserInfoByUserId(const std::string &uid, SlotsUser &su) const ;
	
	bool addUser(const std::string &mid, std::string &uid)const ;
	/* Only update table user_info. */
	bool updateUserInfo(UserInfo &ui) const;
	bool updateUserResource(UserResource &ur) const;	
	bool update(SlotsUserPtr su) const;

	bool searchUser(const std::string &field, const std::string &key,
			uint32_t offset, uint32_t size, SlotsUsers &users);
	
	bool getUserMails(const MyString &uid, MyString &offset, MyString &count, UserMails &out);
	bool readMail(const std::string &uid, const std::string &mailId);
	bool delMail(const std::string &uid, const std::string &mailId);
	
	bool getMailAttachment(const std::string &uid, const std::string &mailId);
	bool getFriendsList(const std::string &uid, uint32_t page,
			    uint32_t pageSize, FriendsList &list);
	bool getInviteHistory(const std::string &uid, FHistory &out);
	bool updateFHistory(const std::string &uid, const std::string &key,
			    const std::string &value);

	bool getReward(const std::string &uid);

	bool removeFriend(const std::string &uid, const std::string &tid);
	bool makeFriend(const std::string &uidStr, const std::string &tidStr);	

	static SlotsDB &getInstance();
    private:
	bool getMailInfo(const cgserver::MysqlRows &mails, UserMails &out);
	/* collect single result from table user_info and user_resource*/
	bool collectSlotsUser(const cgserver::MysqlRow &row, SlotsUser &su) const;
	/* collect multi result from table user_info and user_resource*/	
	bool collectSlotsUsers(const cgserver::MysqlRows &rows, SlotsUsers &out) const;
	
        SlotsDB();
	SlotsDB(SlotsDB &);
	cgserver::MysqlClient &_client;
    };
}
#endif
