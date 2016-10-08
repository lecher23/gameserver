#include "file_util.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fstream>
#include <linux/limits.h>
#include <util/common_define.h>

BEGIN_NAMESPACE(cgserver)

static int split_path(char * path, char ** pathes)
{
    char * ptr;
    int i;
    i = 0;
    while( *path != 0 ) {
        ptr = strchr(path, '/');
        if( ptr == NULL ) {
            pathes[i++] = path;
            break;
        }
        *ptr = 0;
        if( path != ptr ) {
            pathes[i++] = path;
        }
        path = ptr + 1;
    }
    return i;
}

std::string get_realpath(const char * path)
{
    char pwd[PATH_MAX + 1];
    char vpath[PATH_MAX + 1];
    int level, level2;
    char * pathes[PATH_MAX / 2 + 1];
    char * pathes2[PATH_MAX / 2 + 1];
    int i;
    if( path == NULL) {
        errno = EINVAL;
        return std::string();
    }
    if( path[0] != '/' ) {
        if( getcwd(pwd, PATH_MAX + 1) == NULL ) {
            return std::string();
        }
        level = split_path(pwd, pathes);
    } else {
        level = 0;
    }
    strcpy(vpath, path);
    level2 = split_path(vpath, pathes2);
    for( i = 0; i < level2; i++ ) {
        if( pathes2[i][0] == 0 || strcmp(pathes2[i], ".") == 0 ) {
        } else if( strcmp(pathes2[i], "..") == 0 ) {
            level = level > 0 ? level - 1 : 0; // i.e: /../../home -> /home
        } else {
            pathes[level++] = pathes2[i];
        }
    }
    std::string rpath("/");
    for( i = 0; i < level - 1; i++ ) {
        rpath += pathes[i];
        rpath += "/";
    }
    if (level > 0) // return '/' only
        rpath += pathes[i];
    return rpath;
}

int cmp_path(const char * p1, const char * p2)
{
    size_t l1 = strlen(p1);    
    size_t l2 = strlen(p2);
    size_t len = l1 < l2 ? l1 : l2;
    int ok = -1;
    if (l1 > l2) {
        const char * tmp = p1;
        p1 = p2;
        p2 = tmp;
        ok = 1;
    }
    if (strncmp(p1, p2, len) != 0)
        return 0;
    return (p2[len] == 0 || p2[len] == '/') ? ok : 0;
}

std::string joinFilePath(const std::string &path, const std::string &file)
{
    if (file.empty()) {
        return "";
    }

    if (path.empty()) {
        return file;
    }

    if (path[path.length() - 1] == '/') {
        return path + file;
    }

    return path + '/' + file;
}

bool processFile(const std::string &path,
                 std::function<bool(const char *)> processor)
{
    std::ifstream is(path, std::ifstream::in);
    bool ret = false;
    if (!is) {
        CLOG(ERROR) << "open file:" << path << " failed.";
        return false;
    }
    char *buffer = nullptr;
    do{
        is.seekg(0, is.end);
        int len = is.tellg();
        is.seekg(0, is.beg);

        buffer = new char[len + 1];
        is.read(buffer, len);
        if (is) {
            CLOG(INFO) << "read json file: "<< path << " success.";
        } else {
            CLOG(INFO) << "err, only read " << is.gcount() << " bytes in file: " << path;
            break;
        }
        buffer[len] = '\0';
        if (!processor(buffer)) {
            CLOG(ERROR) << "process file content failed.";
            break;
        }

        ret = true;
        break;
    } while (false);
    is.close();
    if (buffer) {
        delete buffer;
    }
    return ret;
}

END_NAMESPACE
