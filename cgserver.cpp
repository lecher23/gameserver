#include "cgserver.h"
#include <iostream>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace cgserver{
  CgServer::CgServer(){
  }
  CgServer::~CgServer(){
    ;
  }
  void CgServer::test(){
      std::cout << "server start." << std::endl;
      _addr.setInetAddr(IPPROTO_TCP, NULL, 9876);
      _server.startServer(&_addr);
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
    server.test();
    return 1;
}
