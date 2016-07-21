#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H
#include <string>
#include <iostream>
namespace cgserver{
#define _LOG(x) std::cout<<(x)<<std::endl;
#define ANET_PACKET_FLAG 0x416e4574
#define CONNECTION_CLOSE "close"
#define CONNECTION_KEEP_ALIVE "Keep-Alive"
    
    const size_t  MAX_PACKET_LEN = 8192;
    const std::string HTTP_GET = "GET";
    const std::string HTTP_POST = "POST";
}

#endif
