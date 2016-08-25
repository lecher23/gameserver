#ifndef CGSERVER_H_
#define CGSERVER_H_

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <error.h>
#include <sstream>
#include "tcpserver.h"

namespace cgserver{
    class CgServer{
    public:
	CgServer();
	~CgServer();
	void start();
    private:
	static const std::string ConfigFilePath;
	TcpServer _server;
  };
}
#endif
