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
    char data[1024];
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
class ConnProcessSub:public ConnSub<T>{
    virtual void update(std::shared_ptr<T> puber, ConnPacketPtr packet) {
        // if this packet is sync packet, just response to client.
        CLOG(INFO) << "processor.";
        if (std::string(packet->data, packet->dataLen) == "exit") {
            CLOG(INFO) << "exit.";
            puber->writeData("bye");
            puber->closeSocket();
            return;
        }
        // process data
        puber->writeData("recieved\n");
        puber->readDataOnce();
    }
};

template<typename T>
class ConnCreateSub: public ConnSub<T> {
public:
    virtual void update(std::shared_ptr<T> puber, ConnPacketPtr packet) {
        std::string body(packet->data, packet->dataLen);
        if (body == "hello") {
            puber->writeData("Hello.");
            puber->readDataOnce();
            auto processor = std::make_shared<ConnProcessSub<T>>();
            auto index = puber->addWatcher(processor);
            processor->setIndex(index);
            puber->removeWatcher(this->getIndex());
        } else {
            CLOG(INFO) << "bad sync packet.";
            puber->closeSocket();
        }
    }
};

END_NAMESPACE
#endif
