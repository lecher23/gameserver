#ifndef REWARDSERVICE_H
#define REWARDSERVICE_H

#include "iservice.h"
#include "slots/data/slotsdatacenter.h"

BEGIN_NAMESPACE(slots)

class RewardService: public IService{
public:
  RewardService();
  virtual ~RewardService();
  virtual bool doJob(CPacket &packet, CResponse &resp);

private:
  bool getLoginReward(CPacket &packet, int64_t &newFortune);
  bool getAchievementReward(CPacket &packet, ResultFormatter &rf);
  bool recvOnlineReward(CPacket &packet, ResultFormatter &rf);
  bool finishTinyGame(CPacket &packet, ResultFormatter &rf);

  SlotsUserData &_userData;
};

END_NAMESPACE
#endif
