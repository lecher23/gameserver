#ifndef CGSERVER_H_
#define CGSERVER_H_

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <error.h>
#include <sstream>
#include "socket/addrspec.h"
#include "tcpserver.h"

namespace cgserver{
    const int LISTEN_BACKLOG = 256;
    class CgServer{
    public:
	CgServer();
	~CgServer();
	void test();
    private:
	AddrSpec _addr;
	TcpServer _server;
  };
}
#endif
