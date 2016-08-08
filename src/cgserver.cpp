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
	int port;
	if (!cfg.initConfig(ConfigFilePath) || (port = cfg.getListenPort()) < 0) {
	    std::cout << "Init config failed." << std::endl;
	    return;
	}
	
	/*Init Logger*/
	google::InitGoogleLogging("CgServer");
	/*Level can be INFO(0)/WARNNING(1)/ERROR(2)/FATAL(3).*/
	const std::string logLevel =
	    Config::getInstance().getConfigValue("server", "log_level");
	const std::string logDir =
	    Config::getInstance().getConfigValue("server", "log_dir");
	int level = 0;
	if (logLevel == "WARNNING") {
	    level = 1;
	}else if (logLevel == "ERROR") {
	    level = 2;
	}else if (logLevel == "FATAL") {
	    level = 3;
	}
	for (int i = 0; i < 4; ++i) {
	    // close link function
	    google::SetLogSymlink(i, "");
	}
	google::SetStderrLogging(4); 
	//error will flush to file
	google::FlushLogFiles(3);
	if (!logDir.empty()) {
	    std::string logFilePrefix = logDir;
	    // set log min level.
	    if (logDir[logDir.size() - 1] == '/') {
		logFilePrefix.append("log");
	    }else {
		logFilePrefix.append("/log");
	    }
	    google::SetLogDestination(level, logFilePrefix.c_str());
	}
	CLOG(INFO) << "Starting server on port["<< port <<"]" ;
	_server.startServer(port);
    }
}

void err_quit(const std::string &str) {
    CLOG(INFO) << str ;
    exit(1);
}

void daemonize(){
    int fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    //struct sigaction sa;
    umask(0);
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
	err_quit("can't get file limit.");
    }

    if((pid = fork()) < 0) {
	err_quit("fork failed.");
    }else if(pid != 0) {
	exit(0);
    }

    setsid();

    // sa.sa_handler = SIG_IGN;
    // sigmeptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // if (sigaction(SIGHUP, &sa, NULL) < 0) {
    // 	err_quit("cannot ignore SIGHUP");
    // }
    if ((pid = fork()) < 0) {
	err_quit("second fork failed.");
    }else if (pid != 0) {
	exit(0);
    }

    if (chdir("/") < 0)
	err_quit("can not chdir to /");

    // if (rl.rlim_max == RLIM_INFINITY) {
    // 	rl.rlim_max = 1024;
    // }

    // for(int i = 0; i < rl.rlim_max; ++i) {
    // 	close(i);
    // }

    fd0 = open("/logs", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
	exit(1);
    }
}

cgserver::CgServer g_server;

static void sig_func(int signo) {
    if (signo == SIGUSR1) {
	// 10
	CLOG(INFO) << "Stoping server.\n";
	g_server.stop();
	exit(0);
    }
}

int main(int len, char ** args) {
    //daemonize(); 
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
