#include "cgtcpprotol.h"
#include <util/stringutil.h>

BEGIN_NAMESPACE(cgserver)

CgTcpProtol::CgTcpProtol(DataBuffer &buffer)
: _body(buffer), _step(EPS_HEAD)
{
    _bytesExpected = MSG_HEAD_LEN;
    _dataReady = false;
}

CgTcpProtol::~CgTcpProtol(){
}

bool CgTcpProtol::dataReady() {
    return _dataReady;
}

char *CgTcpProtol::getBuffer() {
    return (_step == EPS_HEAD ? _header: _body.getData());
}

int32_t CgTcpProtol::expectBytes() {
    return _bytesExpected;
}

void CgTcpProtol::reset() {
    _step = EPS_HEAD;
    _body.clear();
    _dataReady = false;
}

bool CgTcpProtol::parse() {
    bool ret = false;
    switch(_step) {
    case EPS_HEAD:{
        if (getBodyLen()) {
            ret = true;
            _step = EPS_BODY;
            _dataReady = false;
        }
        break;
    }
    case EPS_BODY:{
        ret = true;
        _step = EPS_HEAD;
        _dataReady = true;
        break;
    }
    }
    return ret;
}

bool CgTcpProtol::getBodyLen() {
    _bytesExpected = 0;
    for (size_t i = 0; i < MSG_HEAD_LEN; ++i) {
        if (_header[i] > '9' || _header[i] < '0') {
            return false;
        }
        _bytesExpected = _bytesExpected * 10 + _header[i] - '0';
    }
    return true;
}

END_NAMESPACE
