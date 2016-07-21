#include "addrspec.h"
#include "databuffer.h"
#include "socketprocessor.h"

namespace cgserver{
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
	SocketProcessor _processor;
    };
}
