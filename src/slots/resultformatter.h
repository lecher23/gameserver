#ifndef RESULTFORMATTER_H
#define RESULTFORMATTER_H

#include <slots/data/slotsdatacenter.h>
#include <slots/data/giftsdata.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>
#include <sstream>

namespace slots{
    typedef rapidjson::StringBuffer SBuf;
    typedef rapidjson::Writer<SBuf> JsonWriter;
    class ResultFormatter{
    public:
        explicit ResultFormatter(SBuf &buffer);
        ~ResultFormatter();

	void formatResult(const UserMails &uMails);
	void formatResult(const SlotsUser &su);
	void formatResult(const SlotsUser &su, const Achievements &cj);
	void formatSimpleResult(bool ok, const std::string &err);
	void formatSimpleResult(bool ok, const std::string &key, int64_t value);
	void formatSimpleResult(bool ok, const std::string &key, int32_t value);
	void formatSimpleResult(bool ok, const std::string &key, const std::string &value);
	void formatSimpleResult(bool ok, const std::string &key, bool value);
	void formatGameResult(GameContext &gc);
	void formatFriendsInfo(const FriendsList &friends);
	void formatFullFriendInfo(const FriendsList &friends, const FHistory &history);
	void formatGiftsInfo(const Gifts &gifts, int64_t giftsVal);
	void formatRankResult
	    (const LeaderBoardRank &rank, int64_t uid, uint32_t start, uint32_t end);
	void formatUserAchievement(const Achievements &cj);

	void formatStatus(bool bOk);
	void formatMailsInfo(const UserMails &uMails);
	void formatMail(const UserMail &uMail);
	void formatUserInfo(const UserInfo &uInfo);
	void formatUserResource(const UserResource &uRes);
	void formatFriendList(const FriendsList &friends);
        void formatLoginInfo(const SlotsUser &su);
    private:
	JsonWriter _writer;
    };
}
#endif
