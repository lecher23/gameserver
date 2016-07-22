#include "socket/addrspec.h"
#include "util/databuffer.h"
#include "socket/socketprocessor.h"
#include "socket/socket.h"


namespace cgserver{
    class TcpServer{
    public:
	TcpServer();
	~TcpServer();
	void startServer(int port);
	bool initServer(int port);
	void stopServer();
    private:
	Socket _socket;
	bool _stop;
	SocketProcessor _processor;
    };
}
