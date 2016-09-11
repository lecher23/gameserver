#ifndef CGSOCKET_H
#define CGSOCKET_H

#include <util/common_define.h>
#include <util/databuffer.h>

BEGIN_NAMESPACE(cgserver)
#define MAX_ALIVE_TIME 30

class CgSocket: public std::enable_shared_from_this<CgSocket> {
public:
    explicit CgSocket(asio_service &service);
    ~CgSocket();

    asio_socket &getSocket();
    void start();

private:
    void closeSocket();
    void afterWrite(const asio_error &err, size_t write_len);
    void afterRead(const asio_error &err, size_t read_len);
    void timeoutCheck(const asio_error &err);
    void process();

    asio_service &_service;
    asio_socket _socket;
    asio_deadline_timer_ptr _timer;

    int64_t _lastHeartBeat;
    int64_t _connStartTime;
    int64_t _connStopTime;
    bool _connected;
    bool _writing;

    DataBuffer _input;
    DataBuffer _output;
};

DF_SHARED_PTR(CgSocket);

END_NAMESPACE
#endif
