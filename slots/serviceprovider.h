#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include "iservice.h"
#include "loginservice.h"
#include "updateservice.h"
#include "mailservice.h"

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
	    }

	    return ret;
	}
    };
}
#endif
