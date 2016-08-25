#include "socket/addrspec.h"
#include "util/threadpool.h"
#include "util/iprocessor.h"
#include "handlers/handlerfactory.h"
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
        bool startRowServer(int port);
        bool startAsyncServer(int port);
	Socket _socket;
	bool _stop;
	IProcessorPtr _processor;
	ThreadPoolPtr _pool;
        IHandler *_handler;
    };
}
