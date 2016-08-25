#ifndef UTIL_STRINGUTIL_H
#define UTIL_STRINGUTIL_H

#include <string>
#include <vector>

namespace cgserver{
    typedef std::vector<std::string> strs_t;

    int findChar(const char *src, char dest) {
	int pos = 0;
	while(*src != '\0' && *src != dest) {
	    ++pos;
	    ++src;
	}
	if (*src == '\0')
	    return -1;
	return pos;
    }

    // make sure src is end with '\0'
    void split(const char *src, const char delimiter, strs_t &out) {
	int pos;
	while((pos = findChar(src, delimiter)) != -1) {
	    if (pos > 0)
		out.push_back(std::string(src, pos));
	    src += pos + 1;
	}
	if (*src != '\0')
	    out.push_back(std::string(src));
    }
    
    void splitString(const std::string &src, const char delimiter, strs_t &out){
	split(src.c_str(), delimiter, out);
    }

    
}
#endif
