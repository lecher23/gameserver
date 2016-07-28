#ifndef HTTPSTREAMER_H_
#define HTTPSTREAMER_H_
#include "../util/databuffer.h"
#include "httppacket.h"

namespace cgserver {

class HTTPPacketParser
{
public:
    HTTPPacketParser();
    bool processData(DataBuffer *dataBuffer, HTTPPacket *packet);
    void debug();
    void reset();
    //StreamingContext* createContext();

    static const size_t URI_LIMIT;
    static const size_t PKG_LIMIT;
    static const size_t HEADERS_LIMIT;
    static const std::string EMPTY_STR;
protected:
    static const char _table[256];
    enum ParseStep {
	PS_START_LINE = 0,
	PS_MESSAGE_HEADER,
	PS_MESSAGE_URI,
	PS_MESSAGE_BODY
    };
private:
    /**
     * return ture if start-line parsing completed.
     * set context broken if needed
     **/
    bool processStartLine(DataBuffer *databuffer, HTTPPacket *packet);

    /**
     * return ture if all headers parsing completed.
     * set context broken if needed
     **/
    bool processHeaders(DataBuffer *databuffer, HTTPPacket *packet);

    bool processHeadersOrTrailers(DataBuffer *databuffer, HTTPPacket *packet);

    bool processPerHeaderLine(char *pstart, char *pend, HTTPPacket *packet);

    bool processBody(DataBuffer *databuffer, HTTPPacket *packet);

    bool processLengthBody(DataBuffer *databuffer, HTTPPacket *packet);

    bool processChunkedBody(DataBuffer *databuffer, HTTPPacket *packet);

    bool processEOFBody(DataBuffer *databuffer, HTTPPacket *packet);

    bool processChunkSize(DataBuffer *databuffer, HTTPPacket *packet);

    // *--Function for parse uri(like /a/b?x=1)--*
    bool processURI(HTTPPacket *packet);
    bool parseURI(const char *uri, HTTPPacket *packet);
    int locateChar(const char *dest, char tar);
    bool parseParam(const char *paramStr, HTTPPacket *packet);
    void parseKV(const char *begin, int kvLen, HTTPPacket *packet);
    
    bool getCRLF(DataBuffer *databuffer);

    char* findNextWhiteSpace(char *begin, char *end);
    char* findNextWhiteSpaceAndReplaceZero(char *begin, char *end);
    char* skipWhiteSpaces(char *begin, char *end);
    bool isValidStatusCode(char *p);
    bool isValidToken(char *begin, char *end);
    bool isValidDigits(const char *dist);
    void replaceZero(char *begin, char *end);
    void trim(char *&begin, char *&end, bool left, bool right);
    bool findCRLF(char *start, char *end, char *&CR,  size_t &length);
    bool messageHeadersEnd(HTTPPacket *packet);
    bool processStatusLine(HTTPPacket *packet, char *start, char *end);
    bool processRequestLine(HTTPPacket *packet, char *start, char *end);
    inline bool isTokenCharacter(unsigned char c) {
            return _table[c];
    }
    ParseStep _step;
    bool _parseFinish;
    size_t _drainedLength;
    size_t _dataLength;
};

}/*end namespace anet*/

#endif /*ANET_HTTPSTREAMER_H_*/
