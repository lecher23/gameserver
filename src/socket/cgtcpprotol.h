#ifndef CGTCPPROTOL_H
#define CGTCPPROTOL_H

#include <util/common_define.h>
#include <util/databuffer.h>

BEGIN_NAMESPACE(cgserver)
#define MSG_HEAD_LEN 4

class CgTcpProtol{
public:
    enum ParseStep{
        EPS_HEAD,
        EPS_BODY,
        EPS_ERR
    };
    CgTcpProtol(DataBuffer &buffer);
    ~CgTcpProtol();

    bool dataReady();
    char *getBuffer();
    int32_t expectBytes();
    void reset();
    bool parse();

private:
    bool getBodyLen();
    int32_t _bytesExpected;
    DataBuffer &_body;
    char _header[MSG_HEAD_LEN + 1];

    ParseStep _step;
    bool _dataReady;
};

END_NAMESPACE
#endif
