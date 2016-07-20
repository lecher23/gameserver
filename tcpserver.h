#include "addrspec.h"

namespace cgserver{
#define MAX_BUFF_SIZE 4096
    class TcpServer{
    public:
	TcpServer();
	~TcpServer();
	void startServer(AddrSpec *addr);
	bool initServer(AddrSpec *addr);
	void processConnection(int fd);
	void stopServer();
    private:
	int _fd;
	bool _stop;
	char _buf[4096];
    };
}
