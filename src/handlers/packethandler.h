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
			     cgserver::HttpResponsePacket &resp);
	virtual bool init();
	void debug();

	static const std::string AppName;
	static const std::string AcceptPrefix;
	static const std::string TargetParam;
	static const std::string RefreshTrigger;
	static const std::string DefaultCode;
	static const std::string SpecialSetting;
    private:
	typedef std::map<std::string, std::string> KVMap;	
	typedef KVMap::iterator KVMapItr;
	std::string _acceptPrefix;
	std::string _triggerPath;
	std::string _targetParam;
	std::string _defaultResponse;
	KVMap _cfg;
    };
}
#endif
