#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <slots/services/iservice.h>
#include <slots/services/loginservice.h>
#include <slots/services/updateservice.h>
#include <slots/services/mailservice.h>
#include <slots/services/gameservice.h>
#include <slots/services/rankservice.h>
#include <slots/services/socialityservice.h>

BEGIN_NAMESPACE(slots)
std::map<std::string, IServicePtr> _routing
        = {{"/slots/login", IServicePtr((IService *)new LoginService)},
           {"/slots/update", IServicePtr((IService *)new UpdateService)},
           {"/slots/mail", IServicePtr((IService *)new MailService)},
           {"/slots/game", IServicePtr((IService *)new GameService)},
           {"/slots/friends", IServicePtr((IService *)new SocialityService)},
           {"/slots/rank", IServicePtr((IService *)new RankService)}};

class ServiceProvider{
 public:
    ServiceProvider();
    ~ServiceProvider();

    static const IServicePtr getService(const std::string &path) {
	return _routing[path];
    }
    /* private: */
    /* 	static const std::map<std::string, IServicePtr> _router; */
};
END_NAMESPACE
#endif
