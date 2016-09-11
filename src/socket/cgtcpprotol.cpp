#include "cgtcpprotol.h"
#include <util/stringutil.h>

BEGIN_NAMESPACE(cgserver)

CgTcpProtol::CgTcpProtol()
:_step(EPS_HEAD)
{
    reset();
}

CgTcpProtol::~CgTcpProtol(){
}

void CgTcpProtol::reset() {
    _step = EPS_HEAD;
    _left = MSG_HEAD_LEN;
}

bool CgTcpProtol::getBodyLen() {
    _left = 0;
    for (size_t i = 0; i < MSG_HEAD_LEN; ++i) {
        if (_header[i] > '9' || _header[i] < '0') {
            return false;
        }
        _left = _left * 10 + _header[i] - '0';
    }
    return true;
}

bool CgTcpProtol::parse(const char *src, int32_t len) {
    return (_step == EPS_HEAD) ? parseHead(src, len): parseBody(src, len);
}

bool CgTcpProtol::parseHead(const char *src, int32_t len) {
    if (len <= 0) {
        return len == 0;
    }
    auto needed = (len <= _left) ? len: _left;
    for (size_t i = 0; i < needed; ++i) {
        _header[MSG_HEAD_LEN - _left] = src[i];
        --_left;
    }
    if (_left == 0) {
        if (!getBodyLen()) {
            return false;
        }
        _curPacket.reserve(_left);
        _step = EPS_BODY;
        return parseBody(src + needed, len - needed);
    }
    return true;
}

bool CgTcpProtol::parseBody(const char *src, int32_t len) {
    if(len <= 0) {
        return len == 0;
    }
    auto needed = (len <= _left) ? len: _left;
    _curPacket.append(src, needed);
    _left -= needed;
    if(_left == 0) {
        _packets.push(_curPacket);
        _curPacket.clear();
        _step = EPS_HEAD;
        _left = MSG_HEAD_LEN;
        return parseHead(src + needed, len - needed);
    }
    return true;
}

std::string CgTcpProtol::nextPacket() {
  auto ret = _packets.front();
  _packets.pop();
  return ret;
}

END_NAMESPACE
