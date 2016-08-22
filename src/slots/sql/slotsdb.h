#ifndef SLOTSDB_H
#define SLOTSDB_H

#include <stdint.h>
#include <string>
#include <mysql/mysqlconnpool.h>
#include <mysql/mysqloperationbase.h>
#include <slots/data/basicdata.h>

namespace slots{
    const std::string gAchievement = "user_achievement";
    const std::string gAchievementDetail = "achievement_item";
    const std::string gGameHistory = "g_history";
    const std::string gFriends = "friends";
    const std::string gFriendHistory = "f_history";
    const std::string gUserInfo = "user_info";
    const std::string gUserResource = "user_resource";
    const std::string gAttachment = "m_attachment";
    const std::string gUserMails = "mail_info";
    const std::string gMailDetail = "mails";
    const std::string gLifeHistory = "history";
    
    class SlotsDB{
    public:
        ~SlotsDB();

	bool getUserInfo(cgserver::MysqlOperationBase *mob, SlotsUser &su) const ;
	bool getUserInfoByMachineId(const std::string &mid, SlotsUser &su) const ;
	bool getUserInfoByUserId(const std::string &uid, SlotsUser &su) const ;
	
	bool addUser(const std::string &mid, std::string &uid)const ;
	bool add(const UserCJ &cj);	
	
	bool updateGameHistory(GameHistory &gh) const;
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
	bool getAttachments(std::map<int32_t, AttachmentPtr> &attch);
	bool getAttachment(const std::string &attchId, Attachment &attch);
	    
	bool getFriendsList(const std::string &uid, uint32_t page,
			    uint32_t pageSize, FriendsList &list);
	bool getInviteHistory(const std::string &uid, FHistory &out);
	bool updateFHistory(const std::string &uid, const std::string &key,
			    const std::string &value);

	bool getReward(const std::string &uid);

	bool removeFriend(const std::string &uid, const std::string &tid);
	bool makeFriend(const std::string &uidStr, const std::string &tidStr);

	bool getRankData(RankType rType, LeaderBoardRank &out);
	bool getGameHistory(const std::string &uid, GameHistory &gd) const;

	bool getUserAchievement(const std::string &uid, Achievements &out);
	bool getUserAchievement(const std::string &uid, const std::string &cjID, UserCJ &out);

	bool getAchivementSetting(CjSettingMap &out);

	static SlotsDB &getInstance();
    private:
	bool collectMailInfo(const cgserver::MysqlRows &mails, UserMails &out);
	/* collect single result from table user_info and user_resource*/
	bool collectSlotsUser(const cgserver::MysqlRow &row, SlotsUser &su) const;
	/* collect multi result from table user_info and user_resource*/	
	bool collectSlotsUsers(const cgserver::MysqlRows &rows, SlotsUsers &out) const;
	bool collectUserHistory(const cgserver::MysqlRows &rows, UserHistory &uh) const;
	bool collectAchievement(const cgserver::MysqlRow &row, UserCJ &out) const;	
	bool collectAchievements(const cgserver::MysqlRows &result, Achievements &out) const;	
	
        SlotsDB();
	SlotsDB(SlotsDB &);
	cgserver::MysqlConnPool &_pool;

    };
}
#endif
