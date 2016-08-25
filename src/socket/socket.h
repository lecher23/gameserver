#ifndef SOCKET_H_
#define SOCKET_H_
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sstream>
#include "addrspec.h"
#include "../util/common_define.h"

namespace cgserver {
const int LISTEN_BACKLOG = 256;

  class IOComponent;
  class ThreadMutex;
class Socket {
public:

    Socket(int family = AF_INET);
    ~Socket();

    bool setAddress (const char *address, const int port);
    /* 
     * Set domain socket address. */
    bool setUnixAddress(const char *unixPath);

    bool setAddrSpec(const char *addrspec);

    bool connect();
    bool reconnect();    

    void close();
    void shutdown();

    bool createUDP();

    /*
     * Set socket fd and ip address
     * @param socketHandle: socket fd
     * @param hostAddress: host address
     */
    void setUp(int socketFd, struct sockaddr *hostAddress, int family = AF_INET);

    /*
     * get socket fd
     *
     * @return fd
     */
    int getSocketFd();

    //IOComponent *getIOComponent();
    //void setIOComponent(IOComponent *ioc);

    int write(const void *data, int len);
    int read(void *data, int len);

    bool setKeepAlive(bool on) {
        return setIntOption(SO_KEEPALIVE, on ? 1 : 0);
    }

    bool setKeepAliveParameter(int idleTime, int keepInterval, int cnt);

    bool setReuseAddress(bool on) {
        return setIntOption(SO_REUSEADDR, on ? 1 : 0);
    }

    //deplay time when there has packet not sent but socket is closed.
    bool setSoLinger (bool doLinger, int seconds);

    bool setTcpNoDelay(bool noDelay);

    bool setIntOption(int option, int value);

    bool setSoBlocking(bool on);

    bool checkSocketFd();

    //int setPriority(CONNPRIORITY priority);

    int getSoError();

    int getPort(bool active = true);

    uint32_t getIpAddress(bool active = true);

    char *getAddr(char *dest, int len, bool active = false);

    static int getLastError() {
        return errno;
    }

    inline bool isAddressValid() {
      return addr.isValidState();
    }

    bool getSockAddr(sockaddr_in &addr, bool active = true);
  
    int getProtocolType(void) {return addr.getProtocolType(); }

    /* Functions restructed from ServerSocket class */
    Socket *accept();
    bool listen(int backlog);

    /* below functions are for statics purpose */
    /* int64_t getBindErrCnt(){return atomic_read(&_bindErrCnt);} */
    /* int64_t getConnectErrCnt(){return atomic_read(&_connectErrCnt);} */
    /* int64_t getWriteErrCnt(){return atomic_read(&_writeErrCnt);} */
    /* int64_t getReadErrCnt(){return atomic_read(&_readErrCnt);} */
    /* int64_t getAcceptErrCnt(){return atomic_read(&_acceptErrCnt);} */
    /* int64_t getListenErrCnt(){return atomic_read(&_listenErrCnt);} */

    void dump(const char *leadchar, std::ostringstream &buf) {
        /* std::string leading(leadchar); */
        /* buf << leading << "socket FD: " << _socketHandle << std::endl; */
        /* char tmp[256]; */
        /* getAddr(tmp, 256); */
        /* buf << leading << "address: " << tmp << std::endl; */
        /* buf << leading << "Bind Err: " << getBindErrCnt()  */
        /*     << "\t" << "Connect Err: " << getConnectErrCnt() */
        /*     << "\t" << "Write Err: " << getWriteErrCnt() << std::endl; */
        /* buf << leading << "Read Err: " << getReadErrCnt()  */
        /*     << "\t" << "Accept Err: " << getAcceptErrCnt() */
        /*     << "\t" << "Listen Err: " << getListenErrCnt() << std::endl; */
    }
    
    bool bindLocalAddress(const char *localIp, uint16_t localPort);
    int getProtocolFamily();
protected:
    int _fd;
    //IOComponent *_iocomponent;
    AddrSpec addr;

private:
    /* atomic64_t _bindErrCnt; */
    /* atomic64_t _connectErrCnt; */
    /* atomic64_t _writeErrCnt; */
    /* atomic64_t _readErrCnt; */
    /* atomic64_t _acceptErrCnt; */
    /* atomic64_t _listenErrCnt; */

    void clearCnt();
    /* void bindErrInc(){atomic_inc(&_bindErrCnt);} */
    /* void connectErrInc(){atomic_inc(&_connectErrCnt);} */
    /* void writeErrInc(){atomic_inc(&_writeErrCnt);} */
    /* void readErrInc(){atomic_inc(&_readErrCnt);} */
    /* void acceptErrInc(){atomic_inc(&_acceptErrCnt);} */
    /* void listernErrInc(){atomic_inc(&_listenErrCnt);} */
};

 DF_SHARED_PTR(Socket);
}

#endif /*SOCKET_H_*/
