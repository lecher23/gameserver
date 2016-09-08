#include "tcpserver.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <handlers/packethandler.h>
#include <socket/socketprocessor.h>
#include <util/task.h>
#include <util/processorfactory.h>
#include <util/luatoolfactory.h>
#include <util/config.h>
#include <mysql/mysqlconnpool.h>
#include <resourcefactory.h>

BEGIN_NAMESPACE(cgserver)

TcpServer::TcpServer():_stop(false), _handler(nullptr){
}

TcpServer::~TcpServer() {
}

/* Init Google Logger.*/
void TcpServer::initLogger(){
}

bool TcpServer::initServer(int port) {
    CLOG(INFO) << "Init mysql pool.";
    if (!MysqlConnPool::getInstance().init()) {
	CLOG(ERROR) << "Init mysql pool failed.";
	return false;
    }

    if(!ResourceFactory::getInstance().init()) {
	return false;
    }

    // auto timer = ResourceFactory::getInstance().getServerTimer();
    // auto st = ResourceFactory::getInstance().getServiceThread();
    // if(!st->start()) {
    //     return false;
    // }
    // no timer
    // auto f = [=](){
    // 	CLOG(INFO) << "Timer worked.";
    // };
    //timer->addTask(f, ServerTimer::EET_10_S);

    _handler = HandlerFactory::getHandler();
    _processor = ProcessorFactory::getProcessor();
    if (!_processor->init((void *)_handler)) {
	return false;
    }
    int poolSize;
    if (!Config::getInstance().getIntValue("server", "threadpool_size", poolSize) ||
	poolSize < 0)
    {
	CLOG(ERROR) << "Invalid threadpoll size:" << poolSize;
	return false;
    }
    std::string file = Config::getInstance().getConfigValue("lua", "path");
    if (!file.empty() && !LuaToolFactory::getInstance().init(poolSize + 2, file)) {
	CLOG(ERROR) << "Init lua factory failed.";
	return false;
    }

    CLOG(INFO) << "Init server success.";
    return true;
}

bool TcpServer::startRawServer(int port) {
    // no longer support
    CLOG(ERROR) << "Raw server is no longer used.";
    return false;
}

bool TcpServer::startAsyncServer(int port) {
    asio_service &service = ResourceFactory::getInstance().getIoService();
    _server.reset(new AsyncServer(service, port));
    _server->start(_handler);
    service.run();
}

void TcpServer::startServer(int port) {
    if (!initServer(port)){
	LOG(ERROR) << "Init Server failed.!";
	return;
    }
    int use_asio;
    if (!Config::getInstance().getIntValue("server", "use_asio", use_asio)) {
        use_asio = 1;
    }
    if (use_asio == 0) {
        startRawServer(port);
    }else {
        startAsyncServer(port);
    }
}

void TcpServer::stopServer() {
    _stop = true;
    if (_pool.get() != NULL) {
        _pool->stop(ThreadPool::STOP_THREAD_ONLY);
    }
    if(_server.get() != NULL) {
        _server->stop();
    }
    asio_service &service = ResourceFactory::getInstance().getIoService();
    service.stop();
    _handler->release();
}

END_NAMESPACE
