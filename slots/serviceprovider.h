#ifndef SERVICEPROVIDER_H
#define SERVICEPROVIDER_H

#include "iservice.h"
#include "loginservice.h"

namespace slots{
    class ServiceProvider{
    public:
        ServiceProvider();
        ~ServiceProvider();

	static IServicePtr getService(const std::string &path) {
	    IServicePtr ret;
	    if (path == "/slots/login"){
		ret.reset((IService *)new LoginService);
	    }
	    return ret;
	}
    };
}
#endif
