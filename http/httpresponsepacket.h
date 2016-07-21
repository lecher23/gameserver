#ifndef HTTP_RESPONSE_PACKET_H
#define HTTP_RESPONSE_PACKET_H
#include <string>
#include <map>
#include "../util/databuffer.h"
namespace cgserver {

typedef std::map<std::string, std::string> STRING_MAP;
typedef STRING_MAP::iterator STRING_MAP_ITER;

#define HTTP_STATUS_OK "HTTP/1.1 200 OK\r\n"
#define HTTP_STATUS_NOTFOUND "HTTP/1.1 404 Not Found\r\n"
#define HTTP_KEEP_ALIVE "Connection: Keep-Alive\r\nKeep-Alive: timeout=10, max=10\r\n"
#define HTTP_CONN_CLOSE "Connection: close\r\n"
#define HTTP_CONTENT_TYPE "Content-Type: text/html\r\n"
#define HTTP_CONTENT_LENGTH "Content-Length: %d\r\n"

class HttpResponsePacket {
public:
    /*
     * 构造函数
     */
    HttpResponsePacket();

    /*
     * 析构函数
     */
    ~HttpResponsePacket();

    /*
     * 计算出数据包的长度
     */
    void countDataLen();

    /*
     * 组装
     */
    bool encode(DataBuffer *output);

    /*
     * 解开
     */
    bool decode(DataBuffer *input);//, PacketHeader *header);

    /*
     * 设置header
     */
    void setHeader(const char *name, const char *value);

    /*
     * 设置状态
     */
    void setStatus(bool status);

    /*
     * 设置内容
     */
    void setBody(const char *body, int len);
    void setBody(const char *body);

    /*
     * 是否keepalive
     */
    void setKeepAlive(bool keepAlive);

private:
    bool _status;                   // 返回的状态, true => 200, false => 404
    char *_body;                    // 返回的内容
    int _bodyLen;                   // 返回内容找长度
    STRING_MAP _headerMap;          // 返回其他头信息
    bool _isKeepAlive;              // 是否keepalive
};

}

#endif

