#ifndef _UTIL_FILE_UTIL_H_
#define _UTIL_FILE_UTIL_H_

#include <string>
#include <functional>

namespace cgserver {
    std::string getRealpath(const char * path);
    // -1: p1 is parent of p2
    // 0: p1 and p2 are differnt
    // 1: p2 is parent of p1
    int cmpPath(const char * p1, const char * p2);
    std::string joinFilePath(const std::string &path,
                             const std::string &file);
    bool processFile(const std::string &path,
                     std::function<bool(const char *)> processor);
}
#endif //_UTIL_FILE_UTIL_H_
