#include "cgserver.h"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include "util/config.h"
#include "util/common_define.h"

namespace cgserver{
    const std::string CgServer::ConfigFilePath = "server.cfg";

    CgServer::CgServer(){
    }

    CgServer::~CgServer(){
	;
    }

    void CgServer::stop() {
	_server.stopServer();
	// close logger
	google::ShutdownGoogleLogging();
    }

    void CgServer::start(){
	/*Read config file.*/
	Config &cfg = Config::getInstance();
	if (!cfg.initConfig(ConfigFilePath)){
	    std::cout << "Init config failed." << std::endl;
	    return;
	}
        initGLog();
        int port = cfg.getServerPort();
	CLOG(INFO) << "Starting server on port["<< port <<"]" ;
	_server.startServer(port);
    }

    void CgServer::initGLog() {
	/*Init Logger*/
	google::InitGoogleLogging("CgServer");
	/*Level can be INFO(0)/WARNNING(1)/ERROR(2)/FATAL(3).*/
	int level = Config::getInstance().getLogLevel();
	for (int i = 0; i < 4; ++i) {
	    // close link function
	    google::SetLogSymlink(i, "");
	}
	google::SetStderrLogging(4); 
	//error will flush to file
	google::FlushLogFiles(2);
        auto logDir = Config::getInstance().getLogDir();
        std::string logFilePrefix = logDir;
        // set log min level.
        if (logDir[logDir.size() - 1] == '/') {
            logFilePrefix.append("log");
        }else {
            logFilePrefix.append("/log");
        }
        google::SetLogDestination(level, logFilePrefix.c_str());
    }
}

void daemonize() {
    int fd;
    if (fork() != 0) exit(0);
    setsid();

    if ((fd = open("/def/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) close(fd);
    }
}

cgserver::CgServer g_server;

static void sig_func(int signo) {
    if (signo == SIGUSR1) {
	// 10
	CLOG(INFO) << "Stoping server.\n";
	g_server.stop();
	//exit(0);
    }
}

int main(int argc, char ** argv) {
    if (argc == 2 && !strcasecmp(argv[1], "--daemonize")) {
        daemonize();
    }
    /*regester signal SIGQUIT*/
    if (signal(SIGUSR1, sig_func) == SIG_ERR) {
	std::cout << "Can not catch SIGKILL.\n";
	return 1;
    }
    if (signal(SIGQUIT, sig_func) == SIG_ERR) {
	std::cout << "Can not catch SIGKILL.\n";
	return 1;
    }
    g_server.start();
    return 0;
}
