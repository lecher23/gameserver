#ifndef CGSERVER_H_
#define CGSERVER_H_

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <error.h>
#include <sstream>
#include <server/httpserver.h>

namespace cgserver{
    class CgServer{
    public:
	CgServer();
	~CgServer();
	void start();
	void stop();
    private:
        void initGLog();

	static const std::string ConfigFilePath;
        std::shared_ptr<HttpServer> _httpServerPtr;
  };
}
#endif
