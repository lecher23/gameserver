#ifndef CGTCPPROTOL_H
#define CGTCPPROTOL_H

#include <util/common_define.h>
#include <util/databuffer.h>
#include <queue>

BEGIN_NAMESPACE(cgserver)
#define MSG_HEAD_LEN 4

class CgTcpProtol{
public:
    enum ParseStep{
        EPS_HEAD,
        EPS_BODY,
        EPS_ERR
    };
    CgTcpProtol();
    ~CgTcpProtol();

    bool dataReady();
    char *getBuffer();
    int32_t expectBytes();
    void reset();

    bool parse(const char *src, int32_t len);
    std::string nextPacket();

private:
    bool getBodyLen();
    bool parseHead(const char *src, int32_t len);
    bool parseBody(const char *src, int32_t len);

    char _header[MSG_HEAD_LEN + 1];
    int32_t _left;
    std::string _curPacket;

    std::queue<std::string> _packets;

    ParseStep _step;
};

END_NAMESPACE
#endif
