#ifndef RESULTFORMATTER_H
#define RESULTFORMATTER_H

#include <slots/data/slotsdatacenter.h>
#include <slots/data/giftsdata.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string>

BEGIN_NAMESPACE(slots)

namespace ResultStr{
  const std::string sUserInfo = "uif";
  const std::string sUserResource = "rs";
  const std::string sUserAchievement = "cj";
  const std::string sRankInfo = "rk";
  const std::string sStatus = "st";
  const std::string sMsg = "msg";
  const std::string sLoginInfo = "li";
  const std::string sMails = "ms";
  const std::string sRoomList = "rl";

  // room info
  const std::string sRoomPrize = "pz";

  // user mail info
  const std::string sMailID = "mi";
  const std::string sMailTitle = "mt";
  const std::string sMailContent = "mc";
  const std::string sMailType = "tp";
  const std::string sCreateTime = "ct";
  const std::string sMailKeepDays = "kd";
  const std::string sReadMail = "rm";
  const std::string sGetMailAttchment = "gt";
  const std::string sDelMail = "dm";

  // status
  const std::string sStatusOK = "OK";
  const std::string sStatusError = "ER";

  // user daily reward
  const std::string sDayReward = "dr";
  const std::string sDayn = "d";
  const std::string sVipReward = "v";
  const std::string sRecvReward = "r";
  const std::string sRunnerID = "ri";

  // user info
  const std::string sUserID = "u";
  const std::string sUserMID = "m";
  const std::string sUserName = "un";
  const std::string sUserAvatar = "ua";
  const std::string sUserGender = "g";
  const std::string sUserCountry = "uc";

  // rank info
  const std::string sValue = "v";
  const std::string sID = "id";
  const std::string sMyRank = "mr";

  // user resource
  const std::string sExp = "e";
  const std::string sFortune = "f";
  const std::string sLevel = "l";
  const std::string sVipLevel = "vl";
  const std::string sVipPoint = "vp";

  // user achievement
  const std::string sAchievementID = "ai";

  // game result info
  const std::string sEarnedInfo = "ei";
  const std::string sRatio = "rto";
  const std::string sGridsInfo = "gds";
  const std::string sFreeTimes = "ft";
};

typedef rapidjson::StringBuffer SBuf;
typedef rapidjson::Writer<SBuf> JsonWriter;

class ResultFormatter{
public:
  explicit ResultFormatter(SBuf &buffer);
  ~ResultFormatter();

  void formatResult(const UserMails &uMails);
  void formatResult(const SlotsUser &su);
  void formatResultWithCj(const SlotsUser &su);
  void formatSimpleResult(bool ok, const std::string &err);
  void formatSimpleResult(bool ok, const std::string &key, int64_t value);
  void formatSimpleResult(bool ok, const std::string &key, int32_t value);
  void formatSimpleResult(bool ok, const std::string &key, const std::string &value);
  void formatSimpleResult(bool ok, const std::string &key, bool value);
  void formatSimpleResultWithFortune(int64_t fortune);
  void formatGameResult(GameContext &gc);
  void formatFriendsInfo(const FriendsList &friends);
  void formatFullFriendInfo(const FriendsList &friends, const FHistory &history);
  void formatGiftsInfo(const Gifts &gifts, int64_t giftsVal);
  void formatRankResult
      (const LeaderBoardRank &rank, int64_t uid, uint32_t start, uint32_t end);
  void formatUserAchievement(const Achievements &cj);
  void formatRoomsInfo(const TSRooms &rooms);

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

END_NAMESPACE
#endif
