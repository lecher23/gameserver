#include "slotshandler.h"
namespace cglogic{
    SlotsHandler::SlotsHandler(){
    }
    SlotsHandler::~SlotsHandler(){
    }

    bool SlotsHandler::process(cgserver::HTTPPacket &packet,
			       cgserver::HttpResponsePacket &resp)
    {
	// borrow lua tool.
	cgserver::LuaToolPtr lua = cgserver::LuaToolFactory::getInstance().borrowTool();
	if (lua.get() == NULL) {
	    return false;
	}
	bool pOk = false;
	do {
	    bool exist;
	    const std::string &uid = packet.getParamValue("uid", exist);
	    if (!exist){
		resp.setBody("Parameter [uid] not exist.");
		break;
	    }
	    if (!lua->exeFunc("entrance", uid, 1)) {
		resp.setBody("Call lua script failed.");	    
		break;
	    }
	    std::string ret;
	    lua->getValue(ret);
	    lua->debug();
	    resp.setBody(ret.c_str());
	    pOk = true;
	}while(0);
	// lua tool must return!
	cgserver::LuaToolFactory::getInstance().returnTool(lua);
	return pOk;
    }

    void SlotsHandler::init() {
	;
    }
}
