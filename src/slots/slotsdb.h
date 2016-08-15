#ifndef SLOTSDB_H
#define SLOTSDB_H

#include <stdint.h>
#include <string>
#include "../mysql/mysqlconnpool.h"
#include "../mysql/mysqloperationbase.h"
#include "slotstables.h"

namespace slots{

    class SlotsDB{
    public:
        ~SlotsDB();

	bool getUserInfo(cgserver::MysqlOperationBase *mob, SlotsUser &su) const ;
	bool getUserInfoByMachineId(const std::string &mid, SlotsUser &su) const ;
	bool getUserInfoByUserId(const std::string &uid, SlotsUser &su) const ;
	
	bool addUser(const std::string &mid, std::string &uid)const ;
	/* Only update table user_info. */
	bool updateUserInfo(UserInfo &ui) const;
	bool updateUserResource(UserResource &ur) const;
	bool updateUserHistory(UserHistory &uhis) const;
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

	bool getRankData(RankType rType, LeaderBoardRank &out);

	static SlotsDB &getInstance();
    private:
	bool getMailInfo(const cgserver::MysqlRows &mails, UserMails &out);
	/* collect single result from table user_info and user_resource*/
	bool collectSlotsUser(const cgserver::MysqlRow &row, SlotsUser &su) const;
	/* collect multi result from table user_info and user_resource*/	
	bool collectSlotsUsers(const cgserver::MysqlRows &rows, SlotsUsers &out) const;
	bool collectUserHistory(const cgserver::MysqlRows &rows, UserHistory &uh) const;	
	
        SlotsDB();
	SlotsDB(SlotsDB &);
	cgserver::MysqlConnPool &_pool;
    };
}
#endif
