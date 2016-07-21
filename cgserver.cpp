#include "cgserver.h"
#include <iostream>

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

int main(int len, char ** args) {
  cgserver::CgServer server;
  server.test();
  return 1;
}
