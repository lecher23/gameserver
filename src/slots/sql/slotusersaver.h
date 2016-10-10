#ifndef SLOTUSERSAVER_H
#define SLOTUSERSAVER_H

#include <mysql/mysqloperationbase.h>
#include <slots/data/basicdata.h>

BEGIN_NAMESPACE(slots)

class SlotUserSaver: cgserver::MysqlOperationBase {
public:
  SlotUserSaver();
  ~SlotUserSaver();

  void setUser(SlotsUserPtr in);

  virtual bool doOperation(MYSQL *conn);

private:
  bool saveUserInfo(MYSQL *conn, UserInfo &uInfo);
  bool saveUserResource(MYSQL *conn, UserResource &uRes);
  bool saveUserHistory(MYSQL *conn, GameHistory &uHis);
  bool saveThemeHistory(MYSQL *conn, const std::string &uid, ThemeHistory &tHis);

  SlotsUserPtr user;
};

END_NAMESPACE
#endif
