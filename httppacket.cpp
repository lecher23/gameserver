#include <assert.h>
#include "httppacket.h"
#include <ctype.h>
#include <memory>
#include <string>
#include <stdio.h> 
#include "common_define.h"

using namespace std;

namespace cgserver {

const char* HTTPPacket::empty = "";

HTTPPacket::HTTPPacket() {
    _type = PT_INVALID;
    _version = HTTP_1_1;
    _method = HM_UNSUPPORTED;
    _extendMethod = NULL;
    _URI = NULL;
    _statusCode = 0;
    _reasonPhrase = NULL;
    _headerLen = 0;
    _body = NULL;
    _bodyLength = 0;
    _capacity = 0;

}

HTTPPacket::~HTTPPacket() {
    if (_URI) ::free(_URI);
    if (_extendMethod) ::free(_extendMethod);
    if (_reasonPhrase) ::free(_reasonPhrase);
    for (HeadersType::iterator it = _headers.begin();
         it != _headers.end(); it++) 
    {
        ::free(it->first);
        ::free(it->second);
    }
}

void HTTPPacket::setPacketType(HTTPPacketType type) {
    if (type < PT_REQUEST || type >= PT_INVALID) {
        _type = PT_INVALID;
    } else {
        _type = type;
    }
}

HTTPPacket::HTTPPacketType HTTPPacket::getPacketType() {
    return _type;
}

    
void HTTPPacket::setVersion(HTTPVersion version) {
    if (version < HTTP_1_0  || version >= HTTP_UNSUPPORTED) {
        _version = HTTP_UNSUPPORTED;
    } else {
        _version = version;
    }
}

HTTPPacket::HTTPVersion HTTPPacket::getVersion() {
    return _version;
}


void HTTPPacket::setMethod(HTTPMethod method) {
    if ( method < HM_OPTIONS || method >= HM_UNSUPPORTED) {
        _method = HM_UNSUPPORTED;
    } else {
        _method = method;
    }
    setPacketType(PT_REQUEST);
}

void HTTPPacket::setMethod(const char *method) {
    assert(method);
//    ANET_LOG(SPAM,"method=|%s|", method);
    if (strcmp(method, "GET") == 0) {
        setMethod(HTTPPacket::HM_GET);
        _LOG("setMethod(HTTPPacket::HM_GET)");
    } else if (strcmp(method, "POST") == 0) {
        setMethod(HTTPPacket::HM_POST);
        _LOG("setMethod(HTTPPacket::HM_POST)");
    } else if (strcmp(method, "HEAD") == 0) {
        setMethod(HTTPPacket::HM_HEAD);
        _LOG("setMethod(HTTPPacket::HM_HEAD)");
    } else if (strcmp(method, "OPTIONS") == 0) {
        setMethod(HTTPPacket::HM_OPTIONS);
        _LOG("setMethod(HTTPPacket::HM_OPTIONS)");
    } else if (strcmp(method, "PUT") == 0) {
        setMethod(HTTPPacket::HM_PUT);
        _LOG("setMethod(HTTPPacket::HM_PUT)");
    } else if (strcmp(method, "DELETE") == 0) {
        setMethod(HTTPPacket::HM_DELETE);
        _LOG("setMethod(HTTPPacket::HM_DELETE)");
    } else if (strcmp(method, "TRACE") == 0) {
        setMethod(HTTPPacket::HM_TRACE);
        _LOG("setMethod(HTTPPacket::HM_TRACE)");
    } else if (strcmp(method, "CONNECT") == 0) {
        setMethod(HTTPPacket::HM_CONNECT);
        _LOG("setMethod(HTTPPacket::HM_CONNECT)");
    } else {
        setMethod(HTTPPacket::HM_UNSUPPORTED);
        _LOG("setMethod(HTTPPacket::HM_UNSUPPORTED)");
        if (_extendMethod != NULL) {
            ::free(_extendMethod);
        }
        _extendMethod = strdup(method);
    }
}

const char* HTTPPacket::getMethodString() {
    if (HTTPPacket::HM_GET == getMethod()) {
        return "GET";
    } else if (HTTPPacket::HM_POST == getMethod()) {
        return "POST";
    } else if (HTTPPacket::HM_HEAD == getMethod()) {
        return "HEAD";
    } else if (HTTPPacket::HM_OPTIONS == getMethod()) {
        return "OPTIONS";
    } else if (HTTPPacket::HM_PUT == getMethod()) {
        return "PUT";
    } else if (HTTPPacket::HM_DELETE == getMethod()) {
        return "DELETE";
    } else if (HTTPPacket::HM_TRACE == getMethod()) {
        return "TRACE";
    } else if (HTTPPacket::HM_CONNECT == getMethod()) {
        return "CONNECT";
    } else {
        return _extendMethod;
    }
}

HTTPPacket::HTTPMethod HTTPPacket::getMethod() {
    return _method;
}

    
void HTTPPacket::setURI(const char *URI) {
    if (_URI) {
        ::free(_URI);
        _URI = NULL;
    }
    if ( NULL == URI) {
        return;
    }
    _URI = strdupTrimed(URI);
}

const char* HTTPPacket::getURI() {
    return _URI;
}
    
bool HTTPPacket::addHeader(const char *key, const char *value) {
    if (NULL == key) return false;
    if (NULL == value) return false; 
//    _headers.erase(key);
    removeHeader(key);
    char *newKey = strdup(key);
    assert(newKey);
    char *newValue = strdup(value);
    assert(newValue);
    (void)_headers.insert(make_pair(newKey, newValue));
    _headerLen += strlen(newKey) + strlen(newValue);
    return true;
}

bool HTTPPacket::removeHeader(const char *key) {
    if (NULL == key) return false;
    HeadersType::iterator it 
        = _headers.find((char*)key);
    char *oldKey = NULL;
    char *oldValue = NULL;
    if (it != _headers.end()) {
        oldKey = it->first;
        oldValue = it->second;
    }
    _headers.erase((char*)key);
    if (oldKey != NULL) {
        _headerLen -= strlen(oldKey) + strlen(oldValue);
        ::free(oldKey);
        ::free(oldValue);
    }
    return true;    
}


const char* HTTPPacket::getHeader(const char *key) {
    if (!key) return NULL;
    HeadersType::const_iterator it = _headers.find((char*)key);
    if (it != _headers.end()) {
//        return (it->second).c_str();
        return it->second;
    }
    return NULL;
}

void HTTPPacket::setReasonPhrase(const char *reasonPhrase) {
    if (_reasonPhrase) {
        ::free(_reasonPhrase);
        _reasonPhrase = NULL;
    }
    if (!reasonPhrase) return;

    _reasonPhrase  = strdupTrimed(reasonPhrase, true, false);
}

const char* HTTPPacket::getReasonPhrase() {
    if (_reasonPhrase) {
        return _reasonPhrase;
    }
    switch (_statusCode) {
    case 200:
        return "OK";
    case 404:
        return "NOT FOUND";
    default:
        return "UNKNOWN";
    }
}

void HTTPPacket::setStatusCode(int statusCode) {
    if (statusCode < 100 || statusCode >= 600) {
        statusCode = 0;
    }
    _statusCode =  statusCode;
    setPacketType(PT_RESPONSE);
}

int HTTPPacket::getStatusCode() {
    return _statusCode;
}

void HTTPPacket::setKeepAlive(bool keepAlive) {
    //_LOG( "isKeepAlive:%d", isKeepAlive());
    if (isKeepAlive() == keepAlive) {
        return ;
    }

    if (HTTP_1_0 == getVersion()) {
        if(keepAlive) {
            addHeader("Connection", CONNECTION_KEEP_ALIVE);
        } else {
            removeHeader("Connection");
        }
    } else {
        if(keepAlive) {
            removeHeader("Connection");
        } else {
            addHeader("Connection", CONNECTION_CLOSE);
        }
    }
}

bool HTTPPacket::isKeepAlive() {
    const char *value = getHeader("Connection");
    if (HTTP_1_0 == getVersion()) {
        _LOG( "version: 1.0");
        if (NULL != value && strcmp(CONNECTION_KEEP_ALIVE, value) == 0) {
            return true;
        } else {
            return false;
        }
    } else {
        _LOG( "version: 1.1");
        if (NULL != value && strcmp(CONNECTION_CLOSE, value) == 0) {
            return false;
        } else {
            return true;
        }
    }    
}


char* HTTPPacket::strdupTrimed(const char *str, bool front, bool end) {
    assert(str);
    const char *p1, *p2;
    int oriLen = strlen(str);
    if (0 == oriLen ) {
        return NULL;
    }
    p1 = str;
    p2 = str + oriLen - 1;

    if (front) {
        while (p1 < p2 && (' ' == *p1)) {
            p1 ++;
        }
    }

    if (end) {
        while (p2 > p1 && (' ' == *p2)) {
            p2 --;
        }
    }
    if (p1 == p2 && ' ' == *p1) {
        //_LOG("origin:|%s|trimed to NULL", str);
        return NULL;
    } else {
        size_t len = p2 - p1 + 1;
        char * data = (char*) malloc(len + 1);
        assert(data);
        strncpy(data, p1, len);
        data[len] = '\0';
//        ANET_LOG(DEBUG,"origin:|%s|;trimed:|%s|;len:|%d|", str, data, len);
        return data;
    }
}

bool HTTPPacket::toLowerCase(char *str, int length) {
    if (!str) return false;
    for (int i = 0; i<length; i++) {
        str[i] = tolower(str[i]);
    }
    return true;
}

bool HTTPPacket::encode(DataBuffer *output) {
    return (encodeStartLine(output) 
            && encodeHeaders(output) 
            && encodeBody(output));
}

bool HTTPPacket::encodeStartLine(DataBuffer *output) {
    if (PT_REQUEST == getPacketType()) {
        const char *methodStr = getMethodString();
        const char *uri = getURI();
        if( NULL == methodStr || NULL == uri) {
            return false;
        }
        output->writeBytes(methodStr, strlen(methodStr));
        output->writeBytes(" ", 1);
        output->writeBytes(uri, strlen(uri));
        output->writeBytes(" ", 1);
        if (HTTP_1_0 == getVersion()) {
            output->writeBytes("HTTP/1.0", 8);
        } else {
            output->writeBytes("HTTP/1.1", 8);
        }
        output->writeBytes("\r\n ", 2);
    } else {
        int statusCode = getStatusCode();
        if (0 == statusCode) {
            return false;
        }
        if (HTTP_1_0 == getVersion()) {
            output->writeBytes("HTTP/1.0", 8);
        } else {
            output->writeBytes("HTTP/1.1", 8);
        }
        output->writeBytes(" ", 1);
        char codeStr[32];
        snprintf(codeStr, 32, "%d", statusCode);
        output->writeBytes(codeStr, 3);
        output->writeBytes(" ", 1);
        const char *reasonPhrase = getReasonPhrase();
        if (NULL != reasonPhrase) {
            output->writeBytes(reasonPhrase, strlen(reasonPhrase));
        }
        output->writeBytes("\r\n", 2);
    }
    return true;
}

bool HTTPPacket::encodeHeaders(DataBuffer *output) {
    const char *key = NULL;
    const char *value = NULL;
    HeadersType::const_iterator it = _headers.begin();
    for (; it != _headers.end(); it ++) {
        //key = it->first.c_str();
        key = it->first;
        //value = it->second.c_str();
        value = it->second;
        output->writeBytes(key, strlen(key));
        output->writeBytes(": ", 2);
        output->writeBytes(value, strlen(value));
        output->writeBytes("\r\n", 2);
    }
    if (NULL == getHeader("Content-Length")) {
        char contentLength[64];
        snprintf(contentLength, 64, "Content-Length: %lu\r\n", _bodyLength);
        output->writeBytes(contentLength, strlen(contentLength));
    }
    output->writeBytes("\r\n", 2);
    return true;
}

bool HTTPPacket::encodeBody(DataBuffer *output) {
    if (_bodyLength > 0) {
        output->writeBytes(_body, _bodyLength);
    }
    return true;
}

int64_t HTTPPacket::getSpaceUsed() {
    return _headerLen + sizeof(HTTPPacket) + getBodyLen();
}

HTTPPacket::ConstHeaderIterator HTTPPacket::headerBegin() {
    return _headers.begin();
}

HTTPPacket::ConstHeaderIterator HTTPPacket::headerEnd() {
    return _headers.end();
}

//common function
bool HTTPPacket::setBody(const char *str, size_t length) {
    if (0 == length) {
        if (_body) ::free(_body);
        _body = NULL;
        _bodyLength = 0;
        _capacity = 0;
        return true;
    }
    int oriLen = _bodyLength;
    _bodyLength = 0;
    if (appendBody(str, length)) {
        return true;
    } else {
        _bodyLength = oriLen;
        return false;
    }
}

bool HTTPPacket::appendBody(const char *str, size_t length) {
    if (0 == length || NULL == str) {
        return false;
    }

    if (_capacity - _bodyLength < length) {
        int tmpSize = (_capacity*2 - _bodyLength) >= length ?
                      _capacity*2 : length*2 + _bodyLength;
        if (!setCapacity(tmpSize)) {
            return false;
        }
    }
    memcpy(_body + _bodyLength, str, length);
    _bodyLength += length;
     return true;
}

const char* HTTPPacket::getBody(size_t &length) const {
    length = _bodyLength;
    return _body;
}

char* HTTPPacket::getBody(size_t &length) {
    length = _bodyLength;
    return _body;
}

const char* HTTPPacket::getBody() const {
    return _body;
}

char* HTTPPacket::getBody() {
    return _body;
}
size_t HTTPPacket::getBodyLen() const{
    return _bodyLength;
}

bool HTTPPacket::setCapacity(size_t capacity) {
    if (capacity < _bodyLength) {
        return false;
    }
    if (capacity == _capacity) {
        return true;
    }
    char *tmp = (char *)realloc(_body, capacity);
    if (!tmp) {
        return false;
    }
    _body = tmp;
    _capacity  = capacity;
    return true;
}

size_t HTTPPacket::getCapacity() {
    return _capacity;
}

void HTTPPacket::setPath(const char *src, int len) {
    _path.assign(src, len);
}

const std::string &HTTPPacket::getPath() {
    return _path;
}

void HTTPPacket::addParam(const std::string &key, const std::string &value) {
    _params[key] = value;
}

const HTTPPacket::KVMap &HTTPPacket::getParams(){
    return _params;
}

const std::string &HTTPPacket::getParamValue(const std::string &key, bool &exist) {
    exist = true;
    KVMapItr it = _params.find(key);
    if (it != _params.end())
	return it->second;
    exist = false;
    return common::EMPTY_STR;
}
    
}/*end namespace anet*/
