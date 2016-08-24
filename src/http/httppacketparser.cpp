#include "httppacketparser.h"
#include "../util/databuffer.h"
#include "../util/common_define.h"
#include <stdio.h>

namespace cgserver {

const size_t HTTPPacketParser::URI_LIMIT = 1024 * 64;
const size_t HTTPPacketParser::PKG_LIMIT = 67108864;
const size_t HTTPPacketParser::HEADERS_LIMIT = 128;
    const std::string HTTPPacketParser::EMPTY_STR = "";
const char HTTPPacketParser::_table[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,0,1,1,1,1,1,0,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

HTTPPacketParser::HTTPPacketParser():
    _step(PS_START_LINE),_parseFinish(false),_drainedLength(0),_dataLength(0){}

void HTTPPacketParser::debug(){
    ;
}

void HTTPPacketParser::reset(){
    _step = PS_START_LINE;
    _parseFinish = false;
    _drainedLength = 0;
    _dataLength = 0;
}
    
bool HTTPPacketParser::processData(DataBuffer *dataBuffer, HTTPPacket *packet) 
{
    switch (_step) {
    case PS_START_LINE:
        if (processStartLine(dataBuffer, packet)) {
            _step = PS_MESSAGE_HEADER;
        } else {
            CLOG(WARNING) << "processStartLine() not finished.";
            break;
        }
    case PS_MESSAGE_HEADER:
        if (processHeaders(dataBuffer, packet)) {
            _step = PS_MESSAGE_BODY;
        } else {
            CLOG(WARNING) << "processHeaders() not finished.";
            break;
        }
    case PS_MESSAGE_BODY:
        if (processBody(dataBuffer, packet)) {
            _step = PS_MESSAGE_URI;	    
        } else {
            CLOG(WARNING) << "processBody() not finished.";	    
	    break;
	}
    case PS_MESSAGE_URI:
	if (processURI(packet)) {
	    _parseFinish = true;
	} else {
            CLOG(WARNING) << "processURI() not finished.";	    
	}
	break;
    default:
        CLOG(WARNING) << "SHOULD NOT come here: DEFAULT";
        break;
    }
    return _parseFinish;
}

bool HTTPPacketParser::processURI(HTTPPacket *packet) {
    const char *uri = packet->getURI();
    if (uri == NULL)
	return true;
    return parseURI(uri, packet);
}

bool HTTPPacketParser::parseURI(const char *uri, HTTPPacket *packet) {
    int uriLen = strlen(uri);
    int qFlag = locateChar(uri, '?');
    if (qFlag <= 0) {
	packet->setPath(uri, uriLen);
	return true;
    }
    packet->setPath(uri, qFlag);
    if(!parseParam(uri + qFlag + 1, packet)){
	CLOG(WARNING) << "parse param from uri [" << uri << "] failed.";
	return false;
    }
    return true;
}

int HTTPPacketParser::locateChar(const char *dest, char tar) {
    int ret = 0;
    while(*dest != '\0') {
	if (*dest == tar) return ret;
	++dest;
	++ret;
    }
    return -1;
}

    // x=1&y=3
bool HTTPPacketParser::parseParam(const char *paramStr, HTTPPacket *packet){
    int prev = 0;
    int pos;
    while((pos = locateChar(paramStr, '&')) >= 0){
	parseKV(paramStr, pos, packet);
	paramStr += pos + 1;
    }
    if (*paramStr != '\0') {
	parseKV(paramStr, strlen(paramStr), packet);
    }
    return true;
}

    //x=1, 3
void HTTPPacketParser::parseKV(const char *begin, int kvLen, HTTPPacket *packet) {
    if (kvLen <= 0)
	return;
    int pos = locateChar(begin, '=');
    if (pos == kvLen -1)
	packet->addParam(std::string(begin, pos), EMPTY_STR);
    else if (pos < 0)
	packet->addParam(std::string(begin, kvLen), EMPTY_STR);
    else
	packet->addParam(std::string(begin, pos), std::string(begin + pos + 1, kvLen - pos -1));
}
    

bool HTTPPacketParser::processStartLine(DataBuffer *databuffer, HTTPPacket *packet) 
{
    char *pdata = databuffer->getData();
    char *pend = pdata + databuffer->getDataLen();
    char *cr = NULL;
    size_t length = 0;
    if (!findCRLF(pdata, pend, cr,  length)) {
        if ((size_t)databuffer->getDataLen() > PKG_LIMIT) {
	    CLOG(WARNING) << "Pkg too large";
        } else {
            CLOG(WARNING) << "start line not completed";
        }
        return false;
    }
    *cr = '\0';
    if (length > PKG_LIMIT) {
	CLOG(WARNING) << "length exceed pkg_limit.";
        return false;
    }

    bool rc = false;
    if (strncmp(pdata, "HTTP/", 5) == 0) {
        rc =  processStatusLine(packet, pdata, cr);
    } else {
        rc =  processRequestLine(packet, pdata, cr);
    }
    databuffer->drainData(length);
    _drainedLength += length;
    return rc;
}

bool HTTPPacketParser::processRequestLine(HTTPPacket *packet, 
                                      char *start, char *end) {
    char *p  = findNextWhiteSpace(start, end);
    if (p == end) {
        CLOG(WARNING) << "Broken stream! Only one token in start line!";
        return false;
    }
    *p = '\0';
    assert(packet);
    if (!isValidToken(start, p)) {
        CLOG(WARNING) << "Broken stream! invalid method:" << start;
        return false;
    }
    packet->setPacketType(HTTPPacket::PT_REQUEST);
    packet->setMethod(start);
    start = skipWhiteSpaces(p + 1, end);
    if ( start == end) {
        CLOG(WARNING) << ("Broken stream! Only one token in request line!");
        return false;
    }
    //now processing URI
    p = findNextWhiteSpaceAndReplaceZero(start, end);
    if (p == end) {
        CLOG(WARNING) << ("Broken stream! Only two tokens in request line!");
        return false;
    }
    *p = '\0';
    if ((size_t)(p - start) > URI_LIMIT) {
	CLOG(WARNING) << ("uri too large.");
        return false;
    }
    packet->setURI(start);
        
    start = skipWhiteSpaces(p + 1, end);
    if (strcmp(start, "HTTP/1.0") == 0 ) {
        packet->setVersion(HTTPPacket::HTTP_1_0);
    } else if (strcmp(start, "HTTP/1.1") == 0) {
        packet->setVersion(HTTPPacket::HTTP_1_1);
    } else {
        packet->setVersion(HTTPPacket::HTTP_UNSUPPORTED);
        CLOG(WARNING) << ("Broken stream? Unsupported HTTP version!");
        return false;
    }
    return true;
}

bool HTTPPacketParser::processStatusLine(HTTPPacket *packet, char *start, char *end) 
{
    if (end - start < 128) {
        CLOG(INFO) << "Status Line: " << start;
    }
    char *p  = findNextWhiteSpace(start, end);
    if (p == end) {
        CLOG(WARNING) << ("Broken stream! Only one token in start line!");
        return false;
    }
    *p = '\0';
    packet->setPacketType(HTTPPacket::PT_RESPONSE);
    if (strcmp(start, "HTTP/1.0") == 0 ) {
        packet->setVersion(HTTPPacket::HTTP_1_0);
    } else if (strcmp(start, "HTTP/1.1") == 0) {
        packet->setVersion(HTTPPacket::HTTP_1_1);
    } else {
        packet->setVersion(HTTPPacket::HTTP_UNSUPPORTED);
        CLOG(WARNING) << ("Broken stream? Unsupported HTTP version!");
        return false;
    }
    start = skipWhiteSpaces(p + 1, end);
    if ( start == end) {
        CLOG(WARNING) << ("Broken stream! Only one token in status line!");
        return false;
    }

    //now processing status-code
    p = findNextWhiteSpace(start, end);
    if (p != end) {
        *p = '\0';
    }
    if (!isValidStatusCode(start)) {
        CLOG(WARNING) << "Broken stream! invalid status code:" << start;
        return false;
    }
    int statusCode = atoi(start);
    packet->setStatusCode(statusCode);
    if (p == end) {
        return true;
    }
    start = skipWhiteSpaces(p + 1, end);
    if (start != end) {
        packet->setReasonPhrase(start);
    }
    return true;
}

/**
 * @ret: true: parser completed! false: parser not complete;
 */
bool HTTPPacketParser::processHeaders(DataBuffer *databuffer, 
                                  HTTPPacket *packet) {
    if (processHeadersOrTrailers(databuffer, packet)) {
        return messageHeadersEnd(packet);
    }
    return false;
}

bool HTTPPacketParser::processBody(DataBuffer *databuffer,
                               HTTPPacket *packet) 
{
    switch (packet->_encodingType) {
    case HTTPPacket::ET_NO_BODY:
        return true;
    case HTTPPacket::ET_LENGTH:
        return processLengthBody(databuffer, packet);
    case HTTPPacket::ET_CHUNKED:
        return processChunkedBody(databuffer, packet);
    case HTTPPacket::ET_EOF:
        return processEOFBody(databuffer, packet);
    default:
        return false;
    }
}

bool HTTPPacketParser::processLengthBody(DataBuffer *databuffer,
                                     HTTPPacket *packet) 
{
    size_t bufferLength = databuffer->getDataLen();
    if (bufferLength >= _dataLength) {//we have enough data
        packet->appendBody(databuffer->getData(), _dataLength);
        databuffer->drainData(_dataLength);
        _drainedLength += _dataLength;
        return true;
    } else {
        packet->appendBody(databuffer->getData(), bufferLength);
        databuffer->clear();
        _dataLength -= bufferLength;
        _drainedLength += bufferLength;
        return false;
    }
}

bool HTTPPacketParser::processEOFBody(DataBuffer *databuffer, 
                                  HTTPPacket *packet) 
{
    packet->appendBody(databuffer->getData(), databuffer->getDataLen());
    databuffer->clear();
    return true;
}

bool HTTPPacketParser::processChunkedBody(DataBuffer *databuffer, 
                                      HTTPPacket *packet) 
{
    // won't implement
    return false;
    // do {
    //     switch (context->_chunkState) {
    //     case HTTPStreamingContext::CHUNK_SIZE:
    //         CLOG(WARNING)(SPAM, "Process Chunk Size");
    //         if (!processChunkSize(databuffer, context)) {
    //             return false;
    //         } 
    //         //use switch() to determinate whether this is the lastChunk;
    //         break;
    //     case HTTPStreamingContext::CHUNK_DATA:
    //         CLOG(WARNING)(SPAM, "Process Chunk Data");
    //         if (!processLengthBody(databuffer,context)) {
    //             CLOG(WARNING)(SPAM, "Process Chunk Data NOT finished");
    //             return false;
    //         }
    //         context->_chunkState = HTTPStreamingContext::CHUNK_DATA_CRLF;
    //         CLOG(WARNING)(SPAM, "Process Chunk Data FINISHED");
    //         break;
    //     case HTTPStreamingContext::CHUNK_DATA_CRLF:
    //         CLOG(WARNING)(SPAM, "Process CRLF");
    //         if (!getCRLF(databuffer, context)) {
    //             CLOG(WARNING)(SPAM, "Process CRLF not finished");
    //             return false;
    //         }
    //         CLOG(WARNING)(SPAM, "Process CRLF FINISHED");
    //         context->_chunkState = HTTPStreamingContext::CHUNK_SIZE;
    //         break;
    //     case HTTPStreamingContext::TRAILER:
    //         CLOG(WARNING)(SPAM, "Process TRAILER");
    //         return processHeadersOrTrailers(databuffer, context);
    //     default:
    //         CLOG(WARNING)(ERROR, "We SHOULD NOT get here!");
    //         return false;
    //     }
    // } while(true);
}

bool HTTPPacketParser::processChunkSize(DataBuffer *databuffer,
                                    HTTPPacket *packet) 
{
    char *pstart = databuffer->getData();
    char *pend = pstart + databuffer->getDataLen();
    char *cr = NULL;
    size_t drainLength = 0;
    if (!findCRLF(pstart, pend, cr, drainLength)) {
        if (databuffer->getDataLen() + _drainedLength > PKG_LIMIT) {
            CLOG(WARNING) << ("Pkg too large");
        } else {
            CLOG(WARNING) << ("chunk size not completed");
        }
        return false;
    }
    *cr = '\0';
    if (_drainedLength + drainLength > PKG_LIMIT) {
	CLOG(WARNING) << ("Pkg too large");
        return false;
    }
    
    pstart = skipWhiteSpaces(pstart, cr);
    int chunkSize = -1;
    char c = 0;
    sscanf(pstart, "%x%c", &chunkSize, &c); 
    if(chunkSize < 0 || (0 != c && ';' != c)) {
	CLOG(WARNING) << "Invalid data in chunk size, " << chunkSize << ", " << c;
	return false;
    }

    _dataLength = chunkSize;
    databuffer->drainData(drainLength);
    _drainedLength += drainLength;
    if (_dataLength == 0) {
        CLOG(INFO) << ("Last Chunk Found!");
        // TODO: _chunkState = HTTPStreamingContext::TRAILER;        
        return true;
    }

    if (_drainedLength + _dataLength > PKG_LIMIT) {
	CLOG(WARNING) << ("Pkg too large");
        return false;
    }
    CLOG(INFO) << ("Chunk Size processing finished!");
    // TODO: _chunkState = HTTPStreamingContext::CHUNK_DATA;
    return true;
}

bool HTTPPacketParser::getCRLF(DataBuffer *databuffer) 
{
    char *pdata = databuffer->getData();
    size_t length = databuffer->getDataLen();
    if (length < 1) {
        return false;
    }
    
    if (length == 1) {
        if ('\n' == *pdata) {
	    databuffer->drainData(1);
	    _drainedLength += 1;
	    return true;
        } else if ('\r' != *pdata) {
            return false;
        } else { // *pdata == '\r'
            databuffer->clear();
            _drainedLength += 1;
            return false;
        }
    } 
    
    if ('\n' == *pdata) {
        databuffer->drainData(1);
        _drainedLength += 1;
        return true;        
    }

    if ('\r' == *pdata && '\n' == *(pdata +1)) {
        databuffer->drainData(2);
        _drainedLength += 2;
        return true;        
    }        
    return false;
}

bool HTTPPacketParser::processHeadersOrTrailers(DataBuffer *databuffer, 
						HTTPPacket *packet)
{
    assert(packet);
    while (1) {
        char *pstart = databuffer->getData();
        char *pend = pstart + databuffer->getDataLen();
        char *cr = NULL;
        size_t drainLength = 0;
        if (!findCRLF(pstart, pend, cr, drainLength)) {
            if (databuffer->getDataLen() + _drainedLength > PKG_LIMIT) {
		CLOG(WARNING) << ("pkg too large");
            } else {
                CLOG(WARNING) << ("header not completed");
            }
            return false;
        }
        if (_drainedLength + drainLength > PKG_LIMIT) {
            return false;
        }
        *cr = '\0';
        if (cr == pstart) { //a empty line
            databuffer->drainData(drainLength);
            _drainedLength += drainLength;
            return true;
        }
        processPerHeaderLine(pstart, cr, packet);
        databuffer->drainData(drainLength);
        _drainedLength += drainLength;
    }
    return false;
}

bool HTTPPacketParser::messageHeadersEnd(HTTPPacket *packet) {
    assert(packet);
    const char *encodingType = packet->getHeader("Transfer-Encoding");
    if (NULL == encodingType) {
        const char *contentLength = packet->getHeader("Content-Length");
        if (NULL == contentLength) {
            if (HTTPPacket::PT_RESPONSE  == packet->getPacketType()) {
                packet->_encodingType = HTTPPacket::ET_EOF;
            } else { //fix ticket #146
                packet->_encodingType = HTTPPacket::ET_NO_BODY;
            }
            return true;
        } else {
            if (!isValidDigits(contentLength) ) {
                CLOG(WARNING) << "error content length: " << contentLength;
                return false;
            }
            int  length = atoi(contentLength);
            if (length > 0) {
                packet->_encodingType = HTTPPacket::ET_LENGTH;
                _dataLength = length;
                if (length + _drainedLength > PKG_LIMIT) {
                    return false;
                }
            } else {
                packet->_encodingType = HTTPPacket::ET_NO_BODY;
            }            
            return true;
        }
    } else {
        if (strcmp("chunked", encodingType) == 0) {
            packet->_encodingType = HTTPPacket::ET_CHUNKED;
            return true;
        } else {
            CLOG(WARNING) << ("error transfer encoding type!");
            return false;
        }
    }

}

bool HTTPPacketParser::findCRLF(char *start, char *end, char *&CR,  size_t &length) {
    length = 0;
    while (start < end) {
        length ++;
        if ('\n' == *start) {
            break;
        }
        start ++;
    }
    if (start == end) {
        return false;
    }

    if (length > 1 && '\r' == *(start - 1)) {
        CR = start -1;
    } else {
        CR = start;
    }
    return true;
}

/**
 * process per HeaderLine
 * will not drain data in the databuffer
 * @ret: true: not broken; false: broken
 */
bool HTTPPacketParser::processPerHeaderLine(char *pstart, char *pend, 
                                        HTTPPacket *packet) 
{
    char *key = pstart;
    char *value = NULL;
    while (pstart < pend) {
        if (':' == *pstart) {
            break;
        }
        pstart ++;
    }
    if (pstart >= pend) {
        CLOG(WARNING) << ("lack \":\" in header");
        return false;
    }
    *pstart = 0;
    pstart ++;
    trim(pstart, pend, true, true);
    replaceZero(pstart, pend);
    value = pstart;
    *pend = 0;
    packet->addHeader(key, value);
    
    return true;
}

void HTTPPacketParser::replaceZero(char *begin, char *end) {
    while (begin < end) {
        if (0 == *begin) {
            *begin = ' ';
        }
        begin ++;
    }
}

void HTTPPacketParser::trim(char *&begin, char *&end, bool left, bool right) {
    if (left) {
        while (begin < end && (' ' == *begin || '\t' == *begin) ) {
            begin ++;
        }
    }
    if (right) {
        while (begin < end && (' ' == *(end-1) || '\t' == *(end-1)) ) {
            end --;
        }
    }
}

char* HTTPPacketParser::findNextWhiteSpace(char *begin, char *end) {
    while (begin < end) {
        if ( ' ' == *begin || '\t' == *begin ) {
            break;
        }
        begin ++;
    }
    return begin;
}

char* HTTPPacketParser::findNextWhiteSpaceAndReplaceZero(char *begin, char *end ) {
    while (begin < end) {
        if ( ' ' == *begin || '\t' == *begin ) {
            break;
        }
        if ('\0' == *begin) {
            *begin = ' ';
        }
        begin ++;
    }
    return begin;
}

char* HTTPPacketParser::skipWhiteSpaces(char *begin, char *end ) {
    while (begin < end && (' ' == *begin || '\t' == *begin )) {
        begin ++;
    }
    return begin;
}

bool HTTPPacketParser::isValidStatusCode(char *p) {
    assert(p);
    if (strlen(p) != 3) {
        return false;
    }
    return  isdigit(p[0]) && isdigit(p[1]) &&isdigit(p[2]);
}

bool HTTPPacketParser::isValidToken(char *begin, char *end) {
    while  (begin < end) {
        if (!isTokenCharacter(*begin)) {
            return false;
        }
        begin ++;
    }
    return true;
}

bool HTTPPacketParser::isValidDigits(const char *dist) {
    for (size_t i = 0; i < strlen(dist); i ++) {
        if (!isdigit(dist[i])) {
            return false;
        }
    }
    return true;
}

}/*end namespace anet*/
