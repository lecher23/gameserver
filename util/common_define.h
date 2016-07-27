#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H
#include <string>
#include <iostream>
#include <memory>

#define DF_SHARED_PTR(T) typedef std::shared_ptr<T> T##Ptr;

namespace common{
#define _LOG(x) std::cout<<(x)<<std::endl;
#define CONNECTION_CLOSE "close"
#define CONNECTION_KEEP_ALIVE "Keep-Alive"
    
    enum ret_t
    {   
	r_succeed = 0,
	r_failed = 1,
	r_nullpoint,
	r_enomem,
	r_outofbounds,
	r_einval,
	r_epthread,
	r_uninitialized,
	r_eintr,
	r_eio,
	r_eaccess,
	r_eagain,
	r_eof,
	r_timeout,
	r_yield_thread,
	r_ignore
    };
    const size_t  MAX_PACKET_LEN = 8192;
    const std::string HTTP_GET = "GET";
    const std::string HTTP_POST = "POST";
    const std::string EMPTY_STR = "";
}

#endif
