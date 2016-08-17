#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include <slots/services/iservice.h>
#include <slots/services/loginservice.h>
#include <slots/services/updateservice.h>
#include <slots/services/mailservice.h>
#include <slots/services/gameservice.h>
#include <slots/services/rankservice.h>
#include <slots/services/socialityservice.h>

namespace slots{
    class ServiceProvider{
    public:
        ServiceProvider();
        ~ServiceProvider();

	static IServicePtr getService(const std::string &path) {
	    /* TODO: Use map instead of choice.*/
	    IServicePtr ret;
	    if (path == "/slots/login"){
		ret.reset((IService *)new LoginService);
	    } else if (path == "/slots/update") {
		ret.reset((IService *)new UpdateService);
	    } else if (path == "/slots/mail") {
		ret.reset((IService *)new MailService);
	    } else if (path == "/slots/game") {
		ret.reset((IService *)new GameService);
	    } else if (path == "/slots/friends") {
		ret.reset((IService *)new SocialityService);
	    } else if (path == "/slots/rank") {
		ret.reset((IService *)new RankService);
	    }
	    return ret;
	}
    /* private: */
    /* 	static const std::map<std::string, IServicePtr> _router; */
    };
}
#endif
