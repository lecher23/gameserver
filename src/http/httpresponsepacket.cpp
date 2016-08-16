#include <stdio.h>
#include "httpresponsepacket.h"
#include "../util/databuffer.h"
namespace cgserver {

const std::string HttpResponsePacket::key = "47611322";
    
HttpResponsePacket::HttpResponsePacket() {
    _status = true;
    _body = NULL;
    _bodyLen = 0;
    _isKeepAlive = false;
}

HttpResponsePacket::~HttpResponsePacket() {
    if (_body) {
        ::free(_body);
    }
}

bool HttpResponsePacket::encode(DataBuffer *output) {
    if (_status) { //HTTP/1.1 200 OK
        output->writeBytes(HTTP_STATUS_OK, strlen(HTTP_STATUS_OK));
    } else { // HTTP/1.1 404 Not Found
        output->writeBytes(HTTP_STATUS_NOTFOUND, strlen(HTTP_STATUS_NOTFOUND));
    }
    //固定字段
    if (_isKeepAlive) {
        output->writeBytes(HTTP_KEEP_ALIVE, strlen(HTTP_KEEP_ALIVE));
    } else {
        output->writeBytes(HTTP_CONN_CLOSE, strlen(HTTP_CONN_CLOSE));
    }
    if (_headerMap.find("Content-Type") == _headerMap.end()) {
        output->writeBytes(HTTP_CONTENT_TYPE, strlen(HTTP_CONTENT_TYPE));
    }
    char tmp[64];
    int len = sprintf(tmp, HTTP_CONTENT_LENGTH, _bodyLen);
    output->writeBytes(tmp, len);

    // 用户自定义长度
    for (STRING_MAP_ITER it=_headerMap.begin(); it!=_headerMap.end(); it++) {
        output->writeBytes(it->first.c_str(), strlen(it->first.c_str()));
        output->writeBytes(": ", 2);
        output->writeBytes(it->second.c_str(), strlen(it->second.c_str()));
        output->writeBytes("\r\n", 2);
    }

    // 空行
    output->writeBytes("\r\n", 2);
    // bodyLen
    output->writeBytes(_body, _bodyLen);
    //assert(_packetHeader._dataLen == output->getDataLen());

    return true;
}

bool HttpResponsePacket::decode(DataBuffer *input) {
    return true;
}

void HttpResponsePacket::setHeader(const char *name, const char *value) {
    if (name[0] == 'C') {
        if (strcmp(name, "Connection") == 0 || strcmp(name, "Content-Length") == 0) {
            return;
        }
    }
    _headerMap[name] = value;
}

void HttpResponsePacket::setStatus(bool status) {
    _status = status;
}

void HttpResponsePacket::encrypt(const char *body, int len){
    size_t i = 0;
    size_t key_len = key.size();
    size_t j = 0;
    for (i = 0; i < len; ++i) {
	_body[i] = char (body[i] + key[j] - '0');
	++j;
	if (j == key_len) j = 0;
    }
}

void HttpResponsePacket::setBody(const char *body, int len) {
    if (body && (len > 0)) {
        _body = (char *) malloc(len);
        assert(_body);
        encrypt(body, len);
        _bodyLen = len;
    }
}


void HttpResponsePacket::setBody(const char *body) {
    int len = strlen(body);
    setBody(body, len);
}

char * HttpResponsePacket::getBody() {
    return _body;
}

void HttpResponsePacket::setBodyLen(int len){
    _bodyLen = len;
}

/*
 * 是否keepalive
 */
void HttpResponsePacket::setKeepAlive(bool keepAlive) {
    _isKeepAlive = keepAlive;
}


}

