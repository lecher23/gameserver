#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <socket/addrspec.h>
#include <util/threadpool.h>
#include <util/iprocessor.h>
#include <socket/socket.h>
#include <handlers/handlerfactory.h>
#include <server/asyncserver.h>

namespace cgserver{
    class TcpServer{
    public:
	TcpServer();
	~TcpServer();
	void startServer(int port);
	bool initServer(int port);
	void initLogger();
	void stopServer();
    private:
        bool startAsyncServer(int port);
        bool startRawServer(int port);

	Socket _socket;
	bool _stop;
	IProcessorPtr _processor;
	ThreadPoolPtr _pool;
	IHandler *_handler;
        AsyncServerPtr _server;
    };
}
#endif
