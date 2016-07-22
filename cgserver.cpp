#include "cgserver.h"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util/config.h"

namespace cgserver{
    const std::string CgServer::ConfigFilePath = "server.cfg";
    
    CgServer::CgServer(){
    }
    CgServer::~CgServer(){
	;
    }
    void CgServer::start(){
	std::cout << "init config." << std::endl;
	//Config::getInstance().flag = 1999;
	Config &cfg = Config::getInstance();
	//cfg.flag = 2000;
	int port;
	//std::cout<<Config::getInstance().flag<<cfg.flag<<std::endl;
	if (!cfg.initConfig(ConfigFilePath) || (port = cfg.getListenPort()) < 0) {
	    std::cout << "Init config failed." << std::endl;
	    return;
	}
	std::cout << "Server on port["<< port <<"] start." << std::endl;
	_server.startServer(port);
    }
}

void err_quit(const std::string &str) {
    std::cout << str << std::endl;
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

int main(int len, char ** args) {
    //daemonize();
    cgserver::CgServer server;
    server.start();
    return 1;
}
