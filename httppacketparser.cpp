#include "httppacketparser.h"
#include "databuffer.h"
#include "common_define.h"
#include <stdio.h>

namespace cgserver {

const size_t HTTPPacketParser::URI_LIMIT = 1024 * 64;
const size_t HTTPPacketParser::PKG_LIMIT = 67108864;
const size_t HTTPPacketParser::HEADERS_LIMIT = 128;
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
    _LOG(_drainedLength);
    _LOG(_dataLength);
}

void HTTPPacketParser::reset(){
    _step = PS_START_LINE;
    _parseFinish = false;
    _drainedLength = 0;
    _dataLength = 0;
}
    
bool HTTPPacketParser::processData(DataBuffer *dataBuffer, HTTPPacket *packet) 
{
    std::cout << "Begin process Data" << std::endl;
    _LOG(dataBuffer->getData());
    switch (_step) {
    case PS_START_LINE:
        _LOG("HTTPStreamingContext::HSS_START_LINE");
        if (processStartLine(dataBuffer, packet)) {
            _LOG("processStartLine finished!");
            _step = PS_MESSAGE_HEADER;
        } else {
            _LOG("processStartLine() not finished.");
            break;
        }
    case PS_MESSAGE_HEADER:
        _LOG("HTTPStreamingContext::HSS_MESSAGE_HEADER");
        if (processHeaders(dataBuffer, packet)) {
            _LOG("processHeaders finished!");
            _step = PS_MESSAGE_BODY;
        } else {
            _LOG("processHeaders() not finished.");
            break;
        }
    case PS_MESSAGE_BODY:
        _LOG("HTTPStreamingContext::HSS_MESSAGE_BODY");
        if (processBody(dataBuffer, packet)) {
            _LOG("processBody finished!");
	    _parseFinish = true;
        }
        break;
    default:
        _LOG("SHOULD NOT come here: DEFAULT");
        break;
    }

    return _parseFinish;
}

bool HTTPPacketParser::processStartLine(DataBuffer *databuffer, HTTPPacket *packet) 
{
    char *pdata = databuffer->getData();
    char *pend = pdata + databuffer->getDataLen();
    char *cr = NULL;
    size_t length = 0;
    if (!findCRLF(pdata, pend, cr,  length)) {
        if ((size_t)databuffer->getDataLen() > PKG_LIMIT) {
	    _LOG("pkg too large");
            //context->setErrorNo(AnetError::PKG_TOO_LARGE);
        } else {
            _LOG("start line not completed");
        }
        return false;
    }
    *cr = '\0';
    if (length > PKG_LIMIT) {
	_LOG("length exceed pkg_limit.")
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
        //context->setErrorNo(AnetError::INVALID_DATA);
        _LOG("Broken stream! Only one token in start line!");
        return false;
    }
    *p = '\0';
    assert(packet);
    if (!isValidToken(start, p)) {
        //context->setErrorNo(AnetError::INVALID_DATA);
        _LOG("Broken stream! invalid method!");
        return false;
    }
    packet->setPacketType(HTTPPacket::PT_REQUEST);
    packet->setMethod(start);
    start = skipWhiteSpaces(p + 1, end);
    if ( start == end) {
        //context->setErrorNo(AnetError::INVALID_DATA);
        _LOG("Broken stream! Only one token in request line!");
        return false;
    }
    //now processing URI
    p = findNextWhiteSpaceAndReplaceZero(start, end);
    if (p == end) {
        //context->setErrorNo(AnetError::INVALID_DATA);
        _LOG("Broken stream! Only two tokens in request line!");
        return false;
    }
    *p = '\0';
    if ((size_t)(p - start) > URI_LIMIT) {
        //context->setErrorNo(AnetError::URI_TOO_LARGE);
	_LOG("uri too large.")
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
        //context->setErrorNo(AnetError::VERSION_NOT_SUPPORT);
        _LOG("Broken stream? Unsupported HTTP version!");
        return false;
    }
    return true;
}

bool HTTPPacketParser::processStatusLine(HTTPPacket *packet, char *start, char *end) 
{
    if (end - start < 128) {
        //_LOG("Status Line: |%s|", start);
	;
    }
    char *p  = findNextWhiteSpace(start, end);
    if (p == end) {
        //context->setErrorNo(AnetError::INVALID_DATA);
        _LOG("Broken stream! Only one token in start line!");
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
        //context->setErrorNo(AnetError::VERSION_NOT_SUPPORT);
        _LOG("Broken stream? Unsupported HTTP version!");
        return false;
    }
    start = skipWhiteSpaces(p + 1, end);
    if ( start == end) {
        //context->setErrorNo(AnetError::INVALID_DATA);
        _LOG("Broken stream! Only one token in status line!");
        return false;
    }

    //now processing status-code
    p = findNextWhiteSpace(start, end);
    if (p != end) {
        *p = '\0';
    }
    if (!isValidStatusCode(start)) {
        //context->setErrorNo(AnetError::INVALID_DATA);
        //_LOG("Broken stream! invalid status code|%s|!", start);
        return false;
    }
    int statusCode = atoi(start);
    packet->setStatusCode(statusCode);
    //ANET_LOG(SPAM,"status code(%d),start(%p),end(%p)", statusCode, p, end);
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
    std::cout << "data len:" << _dataLength << std::endl;
    std::cout << "buffer len:" << bufferLength << std::endl;    
    if (bufferLength >= _dataLength) {//we have enough data
        packet->appendBody(databuffer->getData(), _dataLength);
	std::cout << "appen body finish" << std::endl;
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
    //         _LOG(SPAM, "Process Chunk Size");
    //         if (!processChunkSize(databuffer, context)) {
    //             return false;
    //         } 
    //         //use switch() to determinate whether this is the lastChunk;
    //         break;
    //     case HTTPStreamingContext::CHUNK_DATA:
    //         _LOG(SPAM, "Process Chunk Data");
    //         if (!processLengthBody(databuffer,context)) {
    //             _LOG(SPAM, "Process Chunk Data NOT finished");
    //             return false;
    //         }
    //         context->_chunkState = HTTPStreamingContext::CHUNK_DATA_CRLF;
    //         _LOG(SPAM, "Process Chunk Data FINISHED");
    //         break;
    //     case HTTPStreamingContext::CHUNK_DATA_CRLF:
    //         _LOG(SPAM, "Process CRLF");
    //         if (!getCRLF(databuffer, context)) {
    //             _LOG(SPAM, "Process CRLF not finished");
    //             return false;
    //         }
    //         _LOG(SPAM, "Process CRLF FINISHED");
    //         context->_chunkState = HTTPStreamingContext::CHUNK_SIZE;
    //         break;
    //     case HTTPStreamingContext::TRAILER:
    //         _LOG(SPAM, "Process TRAILER");
    //         return processHeadersOrTrailers(databuffer, context);
    //     default:
    //         _LOG(ERROR, "We SHOULD NOT get here!");
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
            //context->setErrorNo(AnetError::PKG_TOO_LARGE);
        } else {
            _LOG("chunk size not completed");
        }
        return false;
    }
    *cr = '\0';
    //_LOG(SPAM, "Chunk Size Line:|%s|", pstart);
    if (_drainedLength + drainLength > PKG_LIMIT) {
        //context->setErrorNo(AnetError::PKG_TOO_LARGE);
        return false;
    }
    
    pstart = skipWhiteSpaces(pstart, cr);
    int chunkSize = -1;
    char c = 0;
    sscanf(pstart, "%x%c", &chunkSize, &c); 
    if(chunkSize < 0 || (0 != c && ';' != c)) {
	//context->setErrorNo(AnetError::INVALID_DATA);
	//_LOG(WARN,"Invalid data in chunk size, %d,%c", chunkSize, c);
      return false;
    }

    _dataLength = chunkSize;
    databuffer->drainData(drainLength);
    _drainedLength += drainLength;
    //_LOG(SPAM, "Chunk size:%lu", context->_dataLength);
    if (_dataLength == 0) {
        _LOG("Last Chunk Found!");
        // TODO: _chunkState = HTTPStreamingContext::TRAILER;        
        return true;
    }

    if (_drainedLength + _dataLength > PKG_LIMIT) {
        //context->setErrorNo(AnetError::PKG_TOO_LARGE);
        return false;
    }
    _LOG("Chunk Size processing finished!");
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
            //context->setErrorNo(AnetError::INVALID_DATA);
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
    //context->setErrorNo(AnetError::INVALID_DATA);
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
                //context->setErrorNo(AnetError::PKG_TOO_LARGE);
		_LOG("pkg too large");
            } else {
                _LOG("header not completed");
            }
            return false;
        }
        if (_drainedLength + drainLength > PKG_LIMIT) {
            //context->setErrorNo(AnetError::PKG_TOO_LARGE);
            return false;
        }
        _LOG("a header line completed");
        *cr = '\0';
        if (cr == pstart) { //a empty line
            databuffer->drainData(drainLength);
            _drainedLength += drainLength;
            return true;
        }
        processPerHeaderLine(pstart, cr, packet);
        databuffer->drainData(drainLength);
        // context->_headersCount ++;
        // if(context->_headersCount > HTTPPacketParser::HEADERS_LIMIT) {
        //     context->setErrorNo(AnetError::TOO_MANY_HEADERS);
        // }
        _drainedLength += drainLength;
    }
    return false;
}

bool HTTPPacketParser::messageHeadersEnd(HTTPPacket *packet) {
    _LOG("Message-Headers END");
    assert(packet);
    const char *encodingType = packet->getHeader("Transfer-Encoding");
    if (NULL == encodingType) {
        const char *contentLength = packet->getHeader("Content-Length");
        if (NULL == contentLength) {
            if (HTTPPacket::PT_RESPONSE  == packet->getPacketType()) {
                _LOG("Set HTTPStreamingContext::HET_EOF");
                packet->_encodingType = HTTPPacket::ET_EOF;
            } else { //fix ticket #146
                _LOG("Set HTTPStreamingContext::HET_NO_BODY");
                packet->_encodingType = HTTPPacket::ET_NO_BODY;
            }
            return true;
        } else {
            if (!isValidDigits(contentLength) ) {
                //_LOG("error content length! %s", contentLength);
                //context->setErrorNo(AnetError::INVALID_DATA);
                return false;
            }
            int  length = atoi(contentLength);
            if (length > 0) {
                _LOG("Set HTTPStreamingContext::HET_LENGTH");
                packet->_encodingType = HTTPPacket::ET_LENGTH;
                _dataLength = length;
                if (length + _drainedLength > PKG_LIMIT) {
                    //context->setErrorNo(AnetError::PKG_TOO_LARGE);
                    return false;
                }
            } else {
                _LOG("Set HTTPStreamingContext::HET_NO_BODY");
                packet->_encodingType = HTTPPacket::ET_NO_BODY;
            }            
            return true;
        }
    } else {
        if (strcmp("chunked", encodingType) == 0) {
            packet->_encodingType = HTTPPacket::ET_CHUNKED;
            return true;
        } else {
            _LOG("error transfer encoding type!");
            //context->setErrorNo(AnetError::INVALID_DATA);
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
        _LOG("lack \":\" in header");
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
