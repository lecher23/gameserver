#ifndef SLOTSDB_H
#define SLOTSDB_H

#include <stdint.h>
#include <string>
#include <mysql/mysqlconnpool.h>
#include <mysql/mysqloperationbase.h>
#include <slots/data/basicdata.h>
#include <slots/data/gamecontext.h>
#include <slots/model/themecommonconfig.h>

BEGIN_NAMESPACE(slots)
const std::string gAchievement = "user_achievement";
const std::string gFriends = "friends";
const std::string gUserInfo = "user_info";
const std::string gUserResource = "user_resource";
const std::string gAttachment = "attachment";
const std::string gUserMails = "user_mails";
const std::string gMailDetail = "mail_detail";

const std::string gCargoInfo = "cargo_config";
const std::string gThemeCommonConfig = "theme_common_cfg";

const std::string gFriendHistory = "friend_history";
const std::string gGameHistory = "game_history";
const std::string gLifeHistory = "history";
const std::string gThemeHistory = "theme_history";
const std::string gCargoHistory = "cargo_history";

class SlotsDB{
public:
    ~SlotsDB();

    bool getUserData(GameContext &out) const;
    bool initNewUser(const std::string &mid, GameContext &user) const;
    bool isUserExist(const std::string &uid) const;
    bool getUserInfo(cgserver::MysqlOperationBase *mob, SlotsUser &su) const ;
    bool getUserInfoByMachineId(const std::string &mid, SlotsUser &su) const ;
    bool getUserIdByMachineId(const std::string &mid, std::string &uid) const ;
    bool getUserInfoByUserId(const std::string &uid, SlotsUser &su) const ;
    bool getUserMails(const MyString &uid, MyString &offset, MyString &count, UserMails &out);
    bool getMailAttachment(const std::string &uid, const std::string &mailId);
    bool getAttachments(std::map<int32_t, AttachmentPtr> &attch);
    bool getAttachment(const std::string &attchId, Attachment &attch);
    bool getFriendsList(const std::string &uid, uint32_t page,
                        uint32_t pageSize, FriendsList &list);
    bool getInviteHistory(const std::string &uid, FHistory &out);
    bool getReward(const std::string &uid);
    bool getRankData(RankType rType, LeaderBoardRank &out);
    bool getGameHistory(const std::string &uid, GameHistory &gd) const;
    bool getUserAchievement(const std::string &uid, Achievements &out) const;
    bool getUserAchievement(const std::string &uid, const std::string &cjID, UserCJ &out);
    bool recvCjReward(const std::string &uid, const std::string &cjID);
    bool getCargoInfo(CargoInfos &out) const;
    bool getThemeCommonConfig(ThemeCommonConfig &out) const;

    bool addUser(const std::string &mid, std::string &uid)const ;
    bool add(const UserCJ &cj);

    bool updateGameHistory(GameHistory &gh) const;
    //bool updateUserHistory(UserHistory &uhis) const;
    bool update(SlotsUserPtr su) const;
    bool updateFHistory(const std::string &uid, const std::string &key,
                        const std::string &value);

    bool searchUser(const std::string &field, const std::string &key,
                    uint32_t offset, uint32_t size, SlotsUsers &users);
    bool removeFriend(const std::string &uid, const std::string &tid);
    bool makeFriend(const std::string &uidStr, const std::string &tidStr);
    bool readMail(const std::string &uid, const std::string &mailId);
    bool delMail(const std::string &uid, const std::string &mailId);

    static SlotsDB &getInstance();
private:
    bool collectMailInfo(const cgserver::MysqlRows &mails, UserMails &out);
    /* collect multi result from table user_info and user_resource*/
    bool collectSlotsUsers(const cgserver::MysqlRows &rows, SlotsUsers &out) const;
    //bool collectUserHistory(const cgserver::MysqlRows &rows, UserHistory &uh) const;
    bool collectAchievements(const cgserver::MysqlRows &result, Achievements &out) const;

    SlotsDB();
    SlotsDB(SlotsDB &);
    cgserver::MysqlConnPool &_pool;
};

END_NAMESPACE
#endif
