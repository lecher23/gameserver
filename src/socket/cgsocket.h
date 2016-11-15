#ifndef CGSOCKET_H
#define CGSOCKET_H

#include <util/common_define.h>
#include <util/databuffer.h>
#include <socket/cgtcpprotol.h>

BEGIN_NAMESPACE(cgserver)
#define MAX_ALIVE_TIME 30
#define MAX_SOCKET_BUFFER_SIZE 30

#define TCP_PACKET_TYPE_SIZE 1
#define TCP_PACKET_DATA_LEN_SIZE 2

struct TCPPacket{
    char sType[TCP_PACKET_TYPE_SIZE];
    char sData[TCP_PACKET_DATA_LEN_SIZE];
    uint16_t dataLen{0};
    char data[1024];
};

DF_SHARED_PTR(TCPPacket);

class CgSocket: public std::enable_shared_from_this<CgSocket> {
public:
    explicit CgSocket(asio_service &service);
    ~CgSocket();

    asio_socket &getSocket();
    void start();

private:
    void closeSocket();
    void afterWrite(const asio_error &err, size_t write_len);
    void timeoutCheck(const asio_error &err);

    void handleReadType(TCPPacketPtr packet, const asio_error &err, size_t readLen);
    void handleReadDataLen(TCPPacketPtr packet, const asio_error &err, size_t readLen);
    void handleProcessPacket(TCPPacketPtr packet, const asio_error &err, size_t readLen);
    void processPacket(TCPPacketPtr packet);

    asio_service &_service;
    asio_socket _socket;
    asio_deadline_timer_ptr _timer;

    int64_t _lastHeartBeat;
    int64_t _connStartTime;
    int64_t _connStopTime;
    bool _connected;
    bool _writing;

    DataBuffer _output;
};

DF_SHARED_PTR(CgSocket);

END_NAMESPACE
#endif
