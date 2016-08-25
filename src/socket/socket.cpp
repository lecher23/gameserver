#include "socket.h"
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
//#include <anet/filecontrol.h>

namespace cgserver {

Socket::Socket(int family) {
    _fd = -1;
    addr.setProtocolFamily(family);
    addr.setProtocolType((int)SOCK_STREAM);
    clearCnt();
}

Socket::~Socket() {
    //close(); associate with reference count
    shutdown();
}

bool Socket::setAddress (const char *address, const int port) {
    int rc = addr.setInetAddr((int)SOCK_STREAM, address, port);
    return !rc;
}

bool Socket::setAddrSpec (const char *address) {
    int rc = addr.setAddrSpec(address);
    return !rc;
}

bool Socket::setUnixAddress(const char *unixPath){
    int rc = addr.setUnixAddr((int)SOCK_STREAM, unixPath);
    return !rc;
}

bool Socket::checkSocketFd() {
    if (_fd != -1) {
        return true;
    }

    if ((_fd = socket(addr.getProtocolFamily(), 
		      addr.getProtocolType(), 0)) == -1) {
        return false;
    }
    // if (!FileControl::setCloseOnExec(_socketHandle)) {
    //     close();
    //     return false;
    // }
    return true;
}

bool Socket::connect() {
    int rc = 0;
    if (!isAddressValid() || !checkSocketFd()) {
        return false;
    }
    
    if (addr.getProtocolFamily() == AF_UNIX) {
        struct sockaddr_un un;
        memset(&un, 0, sizeof(un));
        un.sun_family = AF_UNIX;
        sprintf(un.sun_path, "%s%d", DOMAIN_SOCK_PATH, getpid());
        /* we need to make sure the file is not there already. */
        unlink(un.sun_path);

        rc = ::bind(_fd,(struct sockaddr *)&un, sizeof(un));
        if (rc < 0){
            CLOG(ERROR)<< "Bind domain socket failed in connect. path:"<<
		addr.getUnixPath() << ", err:" << rc;
            return false;
        }
    }
    rc = ::connect(_fd, addr.getAddr(), addr.getAddrSize());
    
    /* Since the socket is in non-blocking mode, connect will most likely 
     * return EINPROGRESS. This error is not beyond our expectation so
     * we only log the errors other than that. */
    if (rc < 0 && errno != EINPROGRESS) {
        CLOG(ERROR) << "connect socket failed, err: " << errno;
    }

    return (0 == rc);
}

bool Socket::reconnect() {
    close();
    return connect();
}

void Socket::close() {
    if (_fd != -1) {
        char tmp[256];
        getAddr(tmp, 256);
        CLOG(INFO) << "Closing socket, fd=" << _fd << ", addr="<< tmp;
        ::close(_fd);
        _fd = -1;
    }
}

void Socket::shutdown() {
    if (_fd != -1) {
	CLOG(INFO) << "Shutdown socket, fd=" << _fd;
        ::shutdown(_fd, SHUT_RDWR);
	::close(_fd);
    }
}

bool Socket::createUDP() {
    close();
    _fd = socket(AF_INET, SOCK_DGRAM, 0);
    return (_fd != -1);
}

void Socket::setUp(int fd, struct sockaddr *hostAddress, int family) {
    close();
    _fd = fd;
    /* TODO: hard code STREAM type only. */
    addr.setAddr(family, (int)SOCK_STREAM, hostAddress);
}

int Socket::getSocketFd() {
    return _fd;
}

int Socket::write(const void *data, int len) {
    if (_fd == -1) {
        return -1;
    }
    if (data == NULL)
        return -1;

    int res = -1;
    do {
        res = ::write(_fd, data, len);
        if (-1 == res && (errno != EINTR && errno != EAGAIN))
        {
	    std::string em = std::string((char *)data, len);
            CLOG(ERROR) << "Write [" << em << "] failed.";
        }
    } while (res < 0 && errno == EINTR);
    return res;
}

int Socket::read (void *data, int len) {
    if (_fd == -1) {
        return -1;
    }

    if (data == NULL)
       return -1;

    int res = -1;
    do {
        res = ::read(_fd, data, len);
        if (-1 == res && (errno != EINTR && errno != EAGAIN))
        {
            CLOG(ERROR) << "Read data from socket " << _fd << " failed.";
        }
    } while (-1 == res && errno == EINTR);
    return res;
}

bool Socket::setKeepAliveParameter(int idleTime, int keepInterval, int cnt) {
    if (checkSocketFd()) {
        int rc = setsockopt(_fd, SOL_TCP, TCP_KEEPIDLE, &idleTime, sizeof(idleTime));
        if (rc != 0)
        {
            CLOG(ERROR) << "Can not set keep alive idle time for the socket:" <<
		_fd << ", err: " << rc;
            return false;
        }
        rc = setsockopt(_fd, SOL_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval));
        if (rc != 0)
        {
            CLOG(ERROR) << "Can not set keep alive interval for the socket: " <<
		_fd << ", err:" << rc;
            return false;
        }
        rc = setsockopt(_fd, SOL_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
        if (rc != 0)
        {
            CLOG(ERROR) << "Can not set keep alive cnt for the socket: "<<
		_fd << ", err:" << rc;
            return false;
        }
        return true;
    }
    return false;
}

bool Socket::setIntOption (int option, int value) {
    bool rc=false;
    if (checkSocketFd()) {
        rc = (setsockopt(_fd, SOL_SOCKET, option,
                         (const void *)(&value), sizeof(value)) == 0);
    }
    return rc;
}

bool Socket::setSoLinger(bool doLinger, int seconds) {
    bool rc=false;
    struct linger lingerTime;
    lingerTime.l_onoff = doLinger ? 1 : 0;
    lingerTime.l_linger = seconds;
    if (checkSocketFd()) {
        rc = (setsockopt(_fd, SOL_SOCKET, SO_LINGER,
                         (const void *)(&lingerTime), sizeof(lingerTime)) == 0);
    }

    return rc;
}

bool Socket::setTcpNoDelay(bool noDelay) {
    bool rc = false;
    int noDelayInt = noDelay ? 1 : 0;
    if (checkSocketFd()) {
        rc = (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY,
                         (const void *)(&noDelayInt), sizeof(noDelayInt)) == 0);
    }
    return rc;
}

bool Socket::setSoBlocking(bool blockingEnabled) {
    bool rc=false;

    if (checkSocketFd()) {
        int flags = fcntl(_fd, F_GETFL, NULL);
        if (flags >= 0) {
            if (blockingEnabled) {
                flags &= ~O_NONBLOCK; // clear nonblocking
            } else {
                flags |= O_NONBLOCK;  // set nonblocking
            }

            if (fcntl(_fd, F_SETFL, flags) >= 0) {
                rc = true;
            }
        }
    }

    return rc;
}


char *Socket::getAddr(char *dest, int len, bool active) {
    if (addr.getProtocolFamily() == AF_INET) {
        struct sockaddr_in addr;    
        if (!getSockAddr(addr, active)) {
            return NULL;
        }

        unsigned long ad = ntohl(addr.sin_addr.s_addr);
        snprintf(dest, len, "%d.%d.%d.%d:%d",
                 static_cast<int>((ad >> 24) & 255),
                 static_cast<int>((ad >> 16) & 255),
                 static_cast<int>((ad >> 8) & 255),
                 static_cast<int>(ad & 255),
                 ntohs(addr.sin_port));
    }
    else {
        /* Domain socket. */
        snprintf(dest, len, "%s",addr.getUnixPath() );
        dest[len-1] = '\0';
    }
    return dest;
}

int Socket::getPort(bool active) {
    struct sockaddr_in addr;    
    if (getSockAddr(addr, active)) {
        return ntohs(addr.sin_port);
    } else {
        return -1;
    }
}

uint32_t Socket::getIpAddress(bool active) {
    struct sockaddr_in addr;
    if (getSockAddr(addr, active)) {
        return ntohl(addr.sin_addr.s_addr);
    } else {
        return (uint32_t)-1;
    }
}

bool Socket::getSockAddr(sockaddr_in &address, bool active) {
    socklen_t addrLen = sizeof(address);
    if (active) {
        int ret = getsockname(_fd, (sockaddr*)&address, &addrLen);
        if (ret != 0 || addrLen != sizeof(address) ) {
            CLOG(ERROR) << "getsockname() fail! [ret, len, fd]: [" <<
		ret << ", " << addrLen << "," << _fd << "].";
            return false;
        }
    } else {
        address = *addr.getInAddr();
    }
    return true;
}

int Socket::getSoError () {
    if (_fd == -1) {
        return EINVAL;
    }

    int lastError = Socket::getLastError();
    int  soError = 0;
    socklen_t soErrorLen = sizeof(soError);
    if (getsockopt(_fd, SOL_SOCKET, SO_ERROR, (void *)(&soError), &soErrorLen) != 0) {
        return lastError;
    }
    if (soErrorLen != sizeof(soError))
        return EINVAL;

    return soError;
}

/* Functions restructed from ServerSocket class */
Socket *Socket::accept() {
    Socket *handleSocket = NULL;

    UnitedAddr address;
    int addrLen = addr.getAddrSize();
    int fd = ::accept(_fd, (struct sockaddr *)&address, (socklen_t *)&addrLen);

    if (fd >= 0) {
        handleSocket = new Socket();
        //assert(handleSocket);
        handleSocket->setUp(fd, (struct sockaddr *)&address, addr.getProtocolFamily());

        /* special processing for domain socket. */
        if (addr.getProtocolFamily() == AF_UNIX){
            unlink(address.un.sun_path);
            CLOG(INFO) << "In accept, unlink domain socket file: " << address.un.sun_path;
        }
    } else {
        int error = getLastError();
        if (error != EAGAIN) {
            CLOG(ERROR) << "Accept return bad fd. error:" << error;
        }
    }

    return handleSocket;
}

bool Socket::listen(int backlog) {
    if (!isAddressValid()) {
        return false;
    }

    if (!checkSocketFd()) {
        return false;
    }

    setReuseAddress(true);
    setSoLinger(true, 0);

    if (addr.getProtocolFamily() == AF_UNIX){
        /* special logic for unix domain socket. 
         * we need to make sure the domain socket file does not exist already. 
         * the semantics is similar to ReuseAddress. */
        unlink(addr.getUnixPath());
    }
    if (::bind(_fd, addr.getAddr(), addr.getAddrSize()) < 0)
    {
        int error = getLastError();
        CLOG(ERROR) << "bind() failed when start listen. error:" << error;
        return false;
    }

    if (::listen(_fd, backlog) < 0) {
        int error = getLastError();
        CLOG(ERROR) << "listen() failed after bind. error:" << error;	
        return false;
    }

    return true;
}

/* below functions are for statics purpose */
void Socket::clearCnt()
{
}

bool Socket::bindLocalAddress(const char *localIp, uint16_t localPort) {
    if (!checkSocketFd()) {
        return false;
    }

    sockaddr_in  localAddr;
    memset(static_cast<void *>(&localAddr), 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(static_cast<short>(localPort));
    if (localIp == NULL || localIp[0] == '\0') {   
        localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        AddrSpec tmpAddr;
        if (tmpAddr.isIpAddrStr(localIp)) {
            localAddr.sin_addr.s_addr = inet_addr(localIp);
        } else {
            return false;
        }
    }
    if (::bind(_fd, (struct sockaddr *)&localAddr,
               sizeof(localAddr)) < 0) 
    {
        return false;
    }
    return true;
}

int Socket::getProtocolFamily() {
    return addr.getProtocolFamily();
}

}
