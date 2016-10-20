#include "resultformatter.h"
BEGIN_NAMESPACE(slots)
ResultFormatter::ResultFormatter(SBuf &buffer):_writer(buffer){
}
    
ResultFormatter::~ResultFormatter(){
}

#define JSON_KEY(str) _writer.Key(str.data(), str.size());
#define JSON_STRING_VALUE(str) _writer.String(str.data(), str.size());

void ResultFormatter::formatMailList(const UserMails &uMails)  {
    _writer.StartObject();
    formatStatus(true);
    formatMailsInfo(uMails);
    _writer.EndObject();
}

void ResultFormatter::formatUser(const SlotsUser &su) {
    formatStatus(true);
    JSON_KEY(ResultStr::sUserInfo);
    formatUserInfo(su.uInfo);

    JSON_KEY(ResultStr::sUserResource);
    formatUserResource(su.uRes);

    JSON_KEY(ResultStr::sUserAchievement);
    formatUserAchievement(su.uCj);
}

void ResultFormatter::formatResultWithCj(const GameContext &context) {
    _writer.StartObject();
    formatStatus(true);

    formatUser(*context.user);

    JSON_KEY(ResultStr::sLoginInfo);
    formatLoginInfo(context);

    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(bool success, const std::string &err)
{
    _writer.StartObject();
    formatStatus(success);
    JSON_KEY(ResultStr::sMsg);
    JSON_STRING_VALUE(err);
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, int64_t value)
{
    _writer.StartObject();
    formatStatus(success);
    JSON_KEY(key);
    _writer.Int64(value);
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, int32_t value)
{
    _writer.StartObject();
    formatStatus(success);
    JSON_KEY(key);
    _writer.Int(value);
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, const std::string &value)
{
    _writer.StartObject();
    formatStatus(success);
    JSON_KEY(key);
    JSON_STRING_VALUE(value);
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResult(
    bool success, const std::string &key, bool value)
{
    _writer.StartObject();
    formatStatus(success);
    JSON_KEY(key);
    _writer.Bool(value);
    _writer.EndObject();
}

void ResultFormatter::formatSimpleResultWithFortune(int64_t fortune) {
    _writer.StartObject();
    formatStatus(true);
    JSON_KEY(ResultStr::sFortune);
    _writer.Int64(fortune);
    _writer.EndObject();
}

void ResultFormatter::formatGameResult(GameContext &gc)
{
    _writer.StartObject();
    formatStatus(true);

    auto &uRes = gc.user->uRes;
    auto &ret = gc.gameInfo;
    JSON_KEY(ResultStr::sFortune);
    _writer.Int64(uRes.fortune);

    JSON_KEY(ResultStr::sEarnedInfo);
    _writer.StartArray();
    _writer.Int64(ret.earned.normal);
    _writer.Int64(ret.earned.roomPrize);
    _writer.Int64(ret.earned.hallPrize);
    _writer.EndArray();

    JSON_KEY(ResultStr::sUserResource);
    formatUserResource(uRes);

    JSON_KEY(ResultStr::sRatio);
    _writer.Int(ret.totalRatio);

    JSON_KEY(ResultStr::sGridsInfo);
    _writer.StartArray();
    auto len = ret.gridsData.size();
    for (size_t i = 0; i < len; ++i) {
        _writer.Int(ret.gridsData[i]);
    }
    _writer.EndArray();

    JSON_KEY(ResultStr::sFreeTimes);
    _writer.Int(ret.freeGameTimes);

    JSON_KEY(ResultStr::sUserAchievement);
    _writer.StartArray();
    for (auto &item: gc.userCj) {
        _writer.Int(item.aid);
    }
    _writer.EndArray();

    JSON_KEY(ResultStr::sTinyGame);
    _writer.StartArray();
    for (auto &item: gc.gameInfo.tinyResult) {
        _writer.Int(item);
    }
    _writer.EndArray();

    _writer.EndObject();
}

void ResultFormatter::formatFriendsInfo(const FriendsList &friends) {
    _writer.StartObject();
    formatStatus(true);
    formatFriendList(friends);
    _writer.EndObject();
}

void ResultFormatter::formatFullFriendInfo(
    const FriendsList &friends, const FHistory &history)
{
    _writer.StartObject();
    formatStatus(true);
    formatFriendList(friends);
    // JSON_KEY("invite_count");
    // _writer.Int(history.inviteCount);
    // JSON_KEY("total_reward");	
    // _writer.Int64(history.totalReward);
    // JSON_KEY("cached_reward");	
    // _writer.Int64(history.rewardRemain);
    _writer.EndObject();
}    

void ResultFormatter::formatGiftsInfo(const Gifts &gifts, int64_t giftsVal) {
    _writer.StartObject();
    formatStatus(true);
    // JSON_KEY("gifts");
    // _writer.StartArray();
    // for (auto itr = gifts.begin(); itr != gifts.end(); ++itr) {
    //     _writer.StartObject();
    //     JSON_KEY("uid");
    //     _writer.Int64((int64_t)(*itr)->master);
    //     JSON_KEY("received");
    //     _writer.Bool((int64_t)(*itr)->received);
    //     JSON_KEY("timestamp");
    //     _writer.Int64((*itr)->timestamp);
    //     _writer.EndObject();
    // }
    // _writer.EndArray();
    // JSON_KEY("value");
    // _writer.Int64(giftsVal);
    _writer.EndObject();
}

// [start, end)
void ResultFormatter::formatRankResult(
    const LeaderBoardRank &rank, int64_t uid, uint32_t start, uint32_t end)
{
    _writer.StartObject();
    formatStatus(true);
    JSON_KEY(ResultStr::sRankInfo);
    _writer.StartArray();
    uint32_t cur = 0;
    int32_t rankValue = 0;
    for (auto &itr: rank.data) {
	if (cur >= start && cur < end) {
	    _writer.StartObject();
	    JSON_KEY(ResultStr::sUserID);
	    _writer.Int64(itr->uid);
	    JSON_KEY(ResultStr::sUserName);
	    JSON_STRING_VALUE(itr->name);
	    JSON_KEY(ResultStr::sUserCountry);
	    _writer.Int(itr->country);
	    JSON_KEY(ResultStr::sValue);
	    _writer.Int64(itr->value);
	    _writer.EndObject();
	}
	++cur;
	if (itr->uid == uid) {
	    rankValue = cur;
	}
    }
    _writer.EndArray();
    JSON_KEY(ResultStr::sMyRank);
    _writer.Int(rankValue);
    _writer.EndObject();
}

void ResultFormatter::formatStatus(bool bOk) {
    JSON_KEY(ResultStr::sStatus);
    if (bOk) {
        JSON_STRING_VALUE(ResultStr::sStatusOK);
    }else {
        JSON_STRING_VALUE(ResultStr::sStatusError);
    }
}

/*"mails":[{..mail info },{}..]*/
void ResultFormatter::formatMailsInfo(const UserMails &uMails)
{
    JSON_KEY(ResultStr::sMails);
    _writer.StartArray();
    for (auto itr = uMails.begin(); itr != uMails.end(); ++itr) {
	formatMail(*(*itr));
    }
    _writer.EndArray();
}

/*{"id":xxx,...}*/
void ResultFormatter::formatMail(const UserMail &uMail) {
    auto &mi = uMail.mailInfo;
    _writer.StartObject();
    JSON_KEY(ResultStr::sMailID);
    JSON_STRING_VALUE(mi.mailId);
    JSON_KEY(ResultStr::sMailTitle);
    JSON_STRING_VALUE(mi.title);
    JSON_KEY(ResultStr::sMailContent);
    JSON_STRING_VALUE(mi.content);
    AttachmentPtr ap;
    bool exist;
    ap = SlotsDataCenter::instance().getAttach(mi.attachment, exist);
    JSON_KEY(ResultStr::sMailType);
    _writer.Int(exist ? ap->type : 0);
    if (exist) {
	JSON_KEY(ResultStr::sValue);
	_writer.Int64(ap->value);
    }
    JSON_KEY(ResultStr::sCreateTime);
    JSON_STRING_VALUE(uMail.mailInfo.createTime);
    JSON_KEY(ResultStr::sMailKeepDays);
    _writer.Int(uMail.mailInfo.keepDays);
    JSON_KEY(ResultStr::sReadMail);
    _writer.Bool(uMail.bRead);
    JSON_KEY(ResultStr::sGetMailAttchment);
    _writer.Bool(uMail.bGet);
    JSON_KEY(ResultStr::sDelMail);
    _writer.Bool(uMail.bDel);
    _writer.EndObject();
}

/* 
   {"uid": "uid",...}
*/
void ResultFormatter::formatUserInfo(const UserInfo &uInfo) {
    _writer.StartObject();
    JSON_KEY(ResultStr::sUserID);
    JSON_STRING_VALUE(uInfo.uid);
    JSON_KEY(ResultStr::sUserMID);
    JSON_STRING_VALUE(uInfo.mid);
    JSON_KEY(ResultStr::sUserName);
    JSON_STRING_VALUE(uInfo.fname);
    JSON_KEY(ResultStr::sUserAvatar);
    JSON_STRING_VALUE(uInfo.avatar);
    JSON_KEY(ResultStr::sUserGender);
    JSON_STRING_VALUE(uInfo.male);
    JSON_KEY(ResultStr::sUserCountry);
    JSON_STRING_VALUE(uInfo.country);
    _writer.EndObject();
}

/* 
   {"uid": "uid",...}
*/
void ResultFormatter::formatUserResource(const UserResource &uRes) {
    _writer.StartObject();
    JSON_KEY(ResultStr::sUserID);
    JSON_STRING_VALUE(uRes.uid);
    JSON_KEY(ResultStr::sLevel);
    _writer.Uint(uRes.level);
    JSON_KEY(ResultStr::sExp);
    _writer.Uint64(uRes.exp);
    JSON_KEY(ResultStr::sFortune);
    _writer.Uint64(uRes.fortune);
    JSON_KEY(ResultStr::sVipLevel);
    _writer.Uint(uRes.vipLevel);
    JSON_KEY(ResultStr::sVipPoint);
    _writer.Int64(uRes.vipPoint);
    _writer.EndObject();
}

void ResultFormatter::formatUserAchievement(const Achievements &cj) {
    _writer.StartArray();
    for (auto &iCj: cj) {
	_writer.StartObject();
	JSON_KEY(ResultStr::sAchievementID);
	_writer.Int(iCj.aid);
	JSON_KEY(ResultStr::sRecvReward);
	_writer.Bool(iCj.isRecvReward);
	JSON_KEY(ResultStr::sCreateTime);
	_writer.Int64(iCj.time);
	_writer.EndObject();
    }
    _writer.EndArray();
}

void ResultFormatter::formatRoomList(const RoomInfos &rooms) {
    _writer.StartObject();
    JSON_KEY(ResultStr::sRoomList);
    _writer.StartArray();
    for(auto &room: rooms) {
        _writer.StartObject();
        JSON_KEY(ResultStr::sID);
        _writer.Int(room.roomID);
        JSON_KEY(ResultStr::sRoomPrize);
        _writer.Int64(room.totalPrize);
        JSON_KEY(ResultStr::sUserID);
        _writer.Int(room.userID);
        _writer.EndObject();
    }
    _writer.EndArray();
    JSON_KEY(ResultStr::sStatus);
    JSON_STRING_VALUE(ResultStr::sStatusOK);
    _writer.EndObject();
}

void ResultFormatter::formatHallList(
    const std::vector<std::string> &hallIDList, const std::vector<std::string> &hallPrizeList){
    if (hallIDList.size() != hallPrizeList.size()) {
        CLOG(WARNING) << "Hall id list not euqal to hall prize list.";
        return;
    }
    _writer.StartObject();
    JSON_KEY(ResultStr::sRoomList);
    int index = 0;
    _writer.StartArray();
    for(auto &hallID: hallIDList) {
        _writer.StartObject();
        JSON_KEY(ResultStr::sID);
        JSON_STRING_VALUE(hallID);
        JSON_KEY(ResultStr::sRoomPrize);
        JSON_STRING_VALUE(hallPrizeList[index]);
        ++index;
        _writer.EndObject();
    }
    _writer.EndArray();
    JSON_KEY(ResultStr::sStatus);
    JSON_STRING_VALUE(ResultStr::sStatusOK);
    _writer.EndObject();
}

void ResultFormatter::formatFriendList(const FriendsList &friends) {
    // JSON_KEY(ResultStr::friends);
    // _writer.StartArray();
    // for (auto itr = friends.begin(); itr != friends.end(); ++itr) {
    //     _writer.StartObject();
    //     JSON_KEY(ResultStr::sUserID);
    //     JSON_STRING_VALUE((*itr)->uInfo.uid);
    //     JSON_KEY(ResultStr::avatar);
    //     JSON_STRING_VALUE((*itr)->uInfo.uid);
    //     JSON_KEY(ResultStr::name);
    //     JSON_STRING_VALUE((*itr)->uInfo.fname);
    //     JSON_KEY(ResultStr::golds);
    //     _writer.Int64((*itr)->uRes.fortune);
    //     _writer.EndObject();
    // }
    // _writer.EndArray();
}

void ResultFormatter::formatLoginInfo(const GameContext &context) {
    _writer.StartObject();
    JSON_KEY(ResultStr::sDayn);
    _writer.Int(context.user->uHis.loginDays);
    JSON_KEY(ResultStr::sRecvReward);
    _writer.Bool(context.dailyReward.recved);
    JSON_KEY(ResultStr::sVipReward);
    _writer.Int64(context.dailyReward.vipExtra);
    JSON_KEY(ResultStr::sRunnerID);
    _writer.Int64(context.dailyReward.runnerIdx);
    JSON_KEY(ResultStr::sRunnerVal);
    _writer.Int64(context.dailyReward.runnerReward);
    JSON_KEY(ResultStr::sDayReward);
    _writer.Int64(context.dailyReward.dayReward);
    _writer.EndObject();
}

#undef JSON_KEY
#undef JSON_STRING_VALUE

END_NAMESPACE
