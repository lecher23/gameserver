#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include "iservice.h"

BEGIN_NAMESPACE(slots)
class UpdateService{
public:
  UpdateService();
  virtual ~UpdateService();

  virtual bool doJob(CPacket &packet, CResponse &resp);
private:
  bool updateUserInfo(CPacket &packet);
  void formatResult(bool status, const std::string &msg, std::string &out);	
};
END_NAMESPACE
#endif
