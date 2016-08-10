#ifndef REQUESTVALIDATOR_H
#define REQUESTVALIDATOR_H

#include "../http/httppacket.h"
namespace cgserver{
    class RequestValidator{
    public:
        RequestValidator();
        ~RequestValidator();

	static bool validate(HTTPPacket &packet, const std::string &secret);
    };
}
#endif
