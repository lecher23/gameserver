#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H
#include "httppacket.h"
#include "httpresponsepacket.h"
#include <string>
#include <map>

namespace cglogic{
    class PacketHandler{
    public:
	typedef std::map<std::string, std::string> KVMap;
	typedef KVMap::iterator KVMapItr;	
	bool process(cgserver::HTTPPacket &packet, cgserver::HttpResponsePacket &resp) const;
	void init();
	void debug();

	static const std::string acceptURI;
	const static std::string User_ID;
    private:
	KVMap cfg;
    };
}
#endif
