#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H
#include <string>
#include <iostream>
#include <memory>
#include "glog/logging.h"

#define DF_SHARED_PTR(T) typedef std::shared_ptr<T> T##Ptr;

#ifndef LIKELY
#if __GNUC__ > 2 || __GNUC_MINOR__ >= 96
    #define LIKELY(x)       __builtin_expect(!!(x),1)
    #define UNLIKELY(x)     __builtin_expect(!!(x),0)
    #define EXPECTED(x,y)   __builtin_expect((x),(y))
#else
    #define LIKELY(x)       (x)
    #define UNLIKELY(x)     (x)
    #define EXPECTED(x,y)   (x)
#endif
#endif


#define CLOG(x) LOG(x)

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

    const std::string ERR_LOAD_LUA = "Load lua file failed.";
    const std::string ERR_HTTP_METHOD = "Only support POST.";
    const std::string ERR_HTTP_URI = "Bad URI.";
    const std::string ERR_HTTP_PARAM = "Wrong parameter in URI.";
    const std::string ERR_CALL_LUA = "Call lua script failed.";
}

#endif
