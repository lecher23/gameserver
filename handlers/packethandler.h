#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H
#include "../http/httppacket.h"
#include "../http/httpresponsepacket.h"
#include "ihandler.h"
#include <string>
#include <map>

namespace cglogic{
    class PacketHandler:cgserver::IHandler{
    public:
	virtual ~PacketHandler();
	virtual bool process(cgserver::HTTPPacket &packet,
			     cgserver::HttpResponsePacket &resp) const;
	virtual void init();
	void debug();

	static const std::string acceptURI;
	const static std::string User_ID;
    private:
	typedef std::map<std::string, std::string> KVMap;	
	typedef KVMap::iterator KVMapItr;	
	KVMap cfg;
    };
}
#endif
