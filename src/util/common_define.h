#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

#define BOOST_SYSTEM_NO_DEPRECATED

#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "glog/logging.h"

#define DF_SHARED_PTR(T) typedef std::shared_ptr<T> T##Ptr;
#define BEGIN_NAMESPACE(x) namespace x{
#define END_NAMESPACE  }

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
#define MyString std::string

#define MUTEX_GUARD(x) std::lock_guard<std::mutex> mutex_guard(x);

typedef boost::asio::io_service asio_service;
typedef boost::asio::io_service::work asio_work;
typedef boost::asio::deadline_timer asio_deadline_timer;
typedef boost::system::error_code asio_error;
#define asio_seconds(V) boost::posix_time::seconds(V)
typedef std::shared_ptr<asio_work> asio_work_ptr;
typedef std::shared_ptr<asio_deadline_timer> asio_deadline_timer_ptr;

namespace common{
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
