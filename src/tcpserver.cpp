#include "tcpserver.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include "handlers/packethandler.h"
#include "handlers/handlerfactory.h"
#include "socket/socketprocessor.h"
#include "util/task.h"
#include "util/processorfactory.h"
#include "util/luatoolfactory.h"
#include "util/config.h"
#include "mysql/mysqlclient.h"

namespace cgserver {
    
TcpServer::TcpServer():_stop(false){
}

TcpServer::~TcpServer() {
}

/* Init Google Logger.*/    
void TcpServer::initLogger(){
}   

bool TcpServer::initServer(int port) {
    IHandler *handler = HandlerFactory::getHandler();
    _processor = ProcessorFactory::getProcessor();
    _processor->init((void *)handler);

    /*Init db*/
    std::string dbAddr = Config::getInstance().getConfigValue("db", "addr");
    std::string dbUName = Config::getInstance().getConfigValue("db", "user_name");
    std::string dbUPwd = Config::getInstance().getConfigValue("db", "user_pwd");
    std::string dbDb = Config::getInstance().getConfigValue("db", "db_name");

    if (dbAddr.empty() || dbUName.empty() || dbDb.empty()) {
	CLOG(ERROR) << "Mysql connection info not set under section [db]";
	return false;
    }

    CLOG(INFO) << "Init mysql client on " << dbAddr;
    if (!MysqlClient::getInstance().initClient(dbAddr, dbUName, dbUPwd, dbDb)) {
	CLOG(ERROR) << "Init mysql client failed.";
	return false;
    }

    std::string file = Config::getInstance().getConfigValue("lua", "path");
    int poolSize;
    if (!Config::getInstance().getIntValue("server", "threadpool_size", poolSize) ||
	poolSize < 0)
    {
	CLOG(ERROR) << "Invalid threadpoll size:" << poolSize;
	return false;
    }
    if (!file.empty() && !LuaToolFactory::getInstance().init(poolSize + 2, file)) {
	CLOG(ERROR) << "Init lua factory failed.";
	return false;
    }

    if (!_socket.setAddress(NULL, port)){
	CLOG(ERROR) << "Set socket address failed.";
	return false;
    }
    
    if (!_socket.listen(256)) {
	CLOG(ERROR) << "Listen failed.";
	return false;
    }

    _pool.reset(new ThreadPool(poolSize));
    if (!_pool->start()) {
	CLOG(ERROR) << "Create thread poll failed.";
	return false;
    }
    CLOG(INFO) << "Init server success.";
    return true;
}

void TcpServer::startServer(int port) {
    if (!initServer(port)){
	LOG(ERROR) << "Init Server failed.!";
	return;
    }
    Socket *clientSocket;
    while(!_stop) {
	if ((clientSocket = _socket.accept()) == NULL) {
	    //accept failed.
	    continue;
	}
	SocketPtr tmp(clientSocket);
	Task *task = new Task(tmp, _processor);
	if (_pool->pushTask((Runnable *)task) != ThreadPool::ERROR_NONE) {
	    // release mem
	    delete task;
	}
    }
}

void TcpServer::stopServer() {
    _stop = true;
    _pool->stop(ThreadPool::STOP_THREAD_ONLY);
}

}
