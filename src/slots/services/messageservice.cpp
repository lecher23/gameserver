#include "messageservice.h"

BEGIN_NAMESPACE(slots)

MessageService::MessageService(){
}

MessageService::~MessageService(){
}

bool MessageService::doJob(CPacket &packet, CResponse &resp){
    CLOG(INFO) << "Begin process message.";
    int32_t gType;
    // if (!getIntVal(packet, "type", gType)) {
    //     return false;
    // }
    SBuf bf;
    ResultFormatter rf(bf);
    bool ret = false;
    switch(gType){
    case 1:{
        break;
    }
    default:
        break;
    }
    resp.setBody(bf.GetString());
    return ret;
}

END_NAMESPACE
