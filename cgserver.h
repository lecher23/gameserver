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
	TcpServer _server;
  };
}
#endif
