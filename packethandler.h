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
	bool process(cgserver::HTTPPacket &packet, cgserver::HttpResponsePacket &resp) ;
	bool parseURI(const char *uri);
	int findChar(const char *dest, char tar);
	bool parseParam(const char *paramStr);
	void parseKV(const char *begin, int kvLen);	
	void init();
	void debug();

	const static std::string acceptURI;
    private:
	KVMap cfg;
	KVMap params;
	std::string _uri;
	std::string _errorStr;
    };
}
#endif
