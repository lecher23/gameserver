#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "util/threadpool.h"
#include "handlers/handlerfactory.h"
#include "http/httppacket.h"
#include "http/httpresponsepacket.h"

BEGIN_NAMESPACE(cgserver)

struct HttpHandler;
typedef boost::network::http::server<HttpHandler> HttpServerImpl;
typedef std::shared_ptr<HttpServerImpl> HttpServerImplPtr;
typedef HttpServerImpl::request HttpRequest;
typedef HttpServerImpl::connection_ptr HttpConnPtr;
typedef HttpServerImpl::string_type HttpStr;
typedef HttpServerImpl::options HttpOpts;

struct HttpHandler {
  void operator() (const HttpRequest &request, HttpConnPtr conn);
  void log(const HttpStr &info);

  void setBusinessHandler(IHandler *handler);

 private:
  bool parseURI(const std::string &uri, HTTPPacket &packet);
  bool parseParam(const char *paramStr, int strLen, HTTPPacket &packet);
  bool parseParam(std::string::const_iterator begin, std::string::const_iterator end, HTTPPacket &packet);
  void parseKV(const char *begin, int kvLen, HTTPPacket &packet);
  bool parseKV(std::string::const_iterator begin, std::string::const_iterator end, HTTPPacket &packet);
  int locateChar(const char *dest, char tar);
  std::string::const_iterator locateChar(
      std::string::const_iterator begin, std::string::const_iterator end, char ch);


  IHandler *_handler{nullptr};
};

class HttpServer{
public:
  HttpServer(const std::string &addr, const std::string &port);
  ~HttpServer();

  bool startServer();
  void stopServer();

  bool initResource();

private:
  bool _stop{true};
  IHandler *_handler;

  HttpHandler _httpHandler;
  HttpOpts _options;
  HttpServerImpl _server;
};

END_NAMESPACE
#endif
