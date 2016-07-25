#include "socket/addrspec.h"
#include "util/threadpool.h"
#include "util/iprocessor.h"
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
	void free();
	
	Socket _socket;
	bool _stop;
	IProcessor *_processor;
	ThreadPoolPtr _pool;
    };
}
