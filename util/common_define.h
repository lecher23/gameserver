#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H
#include <string>
#include <iostream>

namespace common{
#define _LOG(x) std::cout<<(x)<<std::endl;    
#define CONNECTION_CLOSE "close"
#define CONNECTION_KEEP_ALIVE "Keep-Alive"
    
    const size_t  MAX_PACKET_LEN = 8192;
    const std::string HTTP_GET = "GET";
    const std::string HTTP_POST = "POST";
    const std::string EMPTY_STR = "";
}

#endif
