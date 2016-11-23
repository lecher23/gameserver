#ifndef FUTURESUB_H
#define FUTURESUB_H

#include "util/common_define.h"

BEGIN_NAMESPACE(cgserver)

#define TCP_PACKET_TYPE_SIZE 1
#define TCP_PACKET_DATA_LEN_SIZE 2

struct ConnPacket {
    char sType[TCP_PACKET_TYPE_SIZE];
    char sData[TCP_PACKET_DATA_LEN_SIZE];
    uint16_t dataLen{0};
    char data[4096];
};
DF_SHARED_PTR(ConnPacket);

template <typename T>
class ConnSub{
public:
    ConnSub() {}
    virtual ~ConnSub() {}
    virtual void update(std::shared_ptr<T> puber, ConnPacketPtr data) = 0;
    void setIndex(size_t index) {_index = index;}
    size_t getIndex() {return _index;}
    size_t _index;
};

template <typename T>
class UploadWatcher:public ConnSub<T>{
    virtual void update(std::shared_ptr<T> puber, ConnPacketPtr packet) {
        if (!_myturn) {
            if (!puber->connEstablished()) {
                puber->closeSocket();
                return;
            }
            _myturn = true;
        }
        CLOG(INFO) << "upload.";
        std::string body(packet->data, packet->dataLen);
        if (body == "exit") {
            CLOG(INFO) << "exit.";
            puber->writeData("bye");
            puber->closeSocket();
            return;
        }
        // process data
        puber->writeData(body);
        puber->readDataOnce();
    }

private:
    bool _myturn{false};
};

template<typename T>
class SyncWatcher: public ConnSub<T> {
public:
    virtual void update(std::shared_ptr<T> puber, ConnPacketPtr packet) {
        if (puber->connEstablished()) {
            puber->writeData("welcome");
            puber->readDataOnce();
            return;
        }
        std::string body(packet->data, packet->dataLen);
        if (body == "hi") {
            puber->writeData("welcome");
            puber->readDataOnce();
            puber->establishConn(1);
        } else {
            CLOG(INFO) << "bad sync packet.";
            puber->closeSocket();
        }
    }
};

END_NAMESPACE
#endif
