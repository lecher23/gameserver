#include "configure.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "file_util.h"

namespace cgserver{
static uint32_t hashString32(const char * str)
{
    uint32_t h;
    if( !str ) {
        return 0U;
    }
    h = 0;
    for( ; *str; str++ ) {
        h = 5 * h + *str;
    }
    return h;
}


#define MAX_CONFIG_KEY_LEN 64
#define MAX_CONFIG_VAL_LEN 255
struct _config_item_t {
    char _key[MAX_CONFIG_KEY_LEN + 1];
    char _val[MAX_CONFIG_VAL_LEN + 1];
    bool _pair;
};

#define MAX_CONFIG_GROUP_SIZE 256
#define MAX_CONFIG_GROUP_SIZE_PRIME 389
struct _config_group_t
{
    char _grp_name[MAX_CONFIG_KEY_LEN + 1];
    uint32_t _item_count;
    uint32_t _cursor;
    config_item_t _items[MAX_CONFIG_GROUP_SIZE];
    uint32_t _mapper[MAX_CONFIG_GROUP_SIZE_PRIME];
    config_group_t * _sibling;
    config_group_t * _child;
};

struct _config_t {
    config_group_t * _grps;
};

config_t * parseConfigString(const char * str) {
    int len, i;
    const char * ptr;
    const char * equal, * p;
    uint32_t h;
    config_t * cfg;
    config_group_t * cur;
    if( !str ) {
        return NULL;
    }
    cfg = (config_t *)malloc(sizeof(config_t));
    if( !cfg ) {
        fprintf(stderr, "out of memory.\n");
        return NULL;
    }
    cur = (config_group_t*)malloc(sizeof(config_group_t));
    if( !cur ) {
        free(cfg);
        fprintf(stderr, "out of memory.\n");
        return NULL;
    }
    memset(cur , 0, sizeof(config_group_t));
    strcpy(cur->_grp_name, "root");
    cfg->_grps = cur;

    while( str ) {
        p = strchr(str, '\n');
        ptr = str;
        if( p ) {
            len = p - str;
            str = p + 1;
        } else {
            len = strlen(str);
            str = NULL;
        }
        while( len > 0 && (isspace(*ptr) || *ptr == '\n' || *ptr == '\r' )) {
            ptr++;
            len--;
        }
        if( *ptr == '#' ) {
            continue;
        }
        for( i = 0; i < len; i++ ) {
            if( ptr[i] == '#' ) {
                len = i;
                break;
            }
        }
        while( len > 0 && (isspace(*(ptr + len - 1)) || *(ptr + len - 1) == '\n' || *(ptr + len - 1) == '\r') ) {
            len--;
        }
        if( len <= 0 ) {
            continue;
        }
        if( *ptr == '[' && *(ptr + len - 1) == ']' ) {
            if( len > MAX_CONFIG_KEY_LEN || len == 2 ) {
                continue;
            }
            for( i = 1; i < len - 1; i++ ) {
                if( !isalpha(ptr[i]) && !isdigit(ptr[i]) && ptr[i] != '_' ) {
                    break;
                }
            }
            if( i != len - 1 ) {
                fprintf(stderr, "config [ERROR] error group name.\n");
                destroyConfig(cfg);
                return NULL;
            }
            config_group_t * grp;
            grp = (config_group_t*)malloc(sizeof(config_group_t));
            if( grp == NULL ) {
                fprintf(stderr, "out of memory.\n");
                destroyConfig(cfg);
                return NULL;
            }
            memset(grp, 0, sizeof(config_group_t));
            memcpy(grp->_grp_name, ptr + 1, len - 2);
            grp->_grp_name[len - 2] = '\0';
            if( cur != cfg->_grps ) {
                cur->_sibling = grp;
            } else {
                cur->_child = grp;
            }
            cur = grp;
            continue;
        }
        if( cur->_item_count >= MAX_CONFIG_GROUP_SIZE ) {
            fprintf(stderr, 
                    "config [ERROR] too many elements in group '%s', max is %d.\n", 
                    cur->_grp_name, MAX_CONFIG_GROUP_SIZE);
            destroyConfig(cfg);
            return NULL;
        }
        if( *ptr == '=' ) {
            fprintf(stderr, "config [WARN] empty key.\n");
            continue;
        }
        equal = NULL;
        for( i = 0; i < len; i++ ) {
            if( ptr[i] == '=' ) {
                equal = ptr + i;
                break;
            }
        }
        if( !equal ) {
            if( len > MAX_CONFIG_VAL_LEN ) {
                fprintf(stderr, "config [WARN] element is too long.\n");
                continue;
            }
            memcpy(cur->_items[cur->_item_count]._val, ptr, len);
            cur->_items[cur->_item_count]._val[len] = '\0';
            cur->_items[cur->_item_count]._pair = false;
            cur->_item_count++;
        } else {
            p = equal - 1;
            while( isspace(*p) || *p == '\n' || *p == '\r' ) {
                p--;
            }
            if( p - ptr + 1 > MAX_CONFIG_KEY_LEN ) {
                fprintf(stderr, "config [WARN] key is too long.\n");
                continue;
            }
            equal++;
            while( len - (equal - ptr) > 0 && (isspace(*equal) || *equal == '\n' || *equal == '\r') ) {
                equal++;
            }
            if( len - (equal - ptr) == 0 ) {
                fprintf(stderr, "config [WARN] empty value.\n");
                continue;
            }
            if( len - (equal - ptr) > MAX_CONFIG_VAL_LEN ) {
                fprintf(stderr, "config [WARN] value is too long.\n");
                continue;
            }
            memcpy(cur->_items[cur->_item_count]._key, ptr, p - ptr + 1);
            cur->_items[cur->_item_count]._key[p - ptr + 1] = '\0';
            memcpy(cur->_items[cur->_item_count]._val, equal, len - (equal - ptr));
            cur->_items[cur->_item_count]._val[len - (equal - ptr)] = '\0';
            cur->_items[cur->_item_count]._pair = true;
            h = hashString32(cur->_items[cur->_item_count]._key) % MAX_CONFIG_GROUP_SIZE_PRIME;
            cur->_item_count++;
            while( 1 ) {
                if( cur->_mapper[h] == 0 ) {
                    cur->_mapper[h] = cur->_item_count;
                    break;
                }
                h++;
                if( h >= MAX_CONFIG_GROUP_SIZE_PRIME ) {
                    h = 0;
                }
            }
        }
    }
    return cfg;
}

config_t * parseConfigFile(const char * file) {
    FILE * fp;
    char buffer[MAX_CONFIG_VAL_LEN + MAX_CONFIG_KEY_LEN + 64];
    int len, size, i, line;
    char * ptr;
    char * equal, * p;
    uint32_t h;
    config_t * cfg;
    config_group_t * cur;
    if( !file ) {
        return NULL;
    }
    fp = fopen(file, "r");
    if( !fp ) {
        return NULL;
    }
    cfg = (config_t *)malloc(sizeof(config_t));
    if( !cfg ) {
        fclose(fp);
        fprintf(stderr, "out of memory.\n");
        return NULL;
    }
    cur = (config_group_t*)malloc(sizeof(config_group_t));
    if( !cur ) {
        free(cfg);
        fclose(fp);
        fprintf(stderr, "out of memory.\n");
        return NULL;
    }
    memset(cur , 0, sizeof(config_group_t));
    strcpy(cur->_grp_name, "root");
    cfg->_grps = cur;
    size = MAX_CONFIG_VAL_LEN + MAX_CONFIG_KEY_LEN + 64;
    line = 0;

    while( fgets(buffer, size, fp) ) {
        line++;
        ptr = buffer;
        while( *ptr && (isspace(*ptr) || *ptr == '\n' || *ptr == '\r' )) {
            ptr++;
        }
        if( *ptr == '#' ) {
            continue;
        }
        len = 0;
        while( *(ptr + len) && *(ptr + len) != '#' ) {
            len++;
        }
        while( len > 0 && (isspace(*(ptr + len - 1)) || *(ptr + len - 1) == '\n' || *(ptr + len - 1) == '\r') ) {
            len--;
        }
        if( len <= 0 ) {
            continue;
        }
        ptr[len] = 0;
        if( *ptr == '[' && *(ptr + len - 1) == ']' ) {
            if( len > MAX_CONFIG_KEY_LEN || len == 2 ) {
                continue;
            }
            for( i = 1; i < len - 1; i++ ) {
                if( !isalpha(ptr[i]) && !isdigit(ptr[i]) && ptr[i] != '_' ) {
                    break;
                }
            }
            if( i != len - 1 ) {
                fprintf(stderr, "config [ERROR] error group name: %s\n", ptr);
                fclose(fp);
                destroyConfig(cfg);
                return NULL;
            }
            config_group_t * grp;
            grp = (config_group_t*)malloc(sizeof(config_group_t));
            if( grp == NULL ) {
                fprintf(stderr, "out of memory.\n");
                fclose(fp);
                destroyConfig(cfg);
                return NULL;
            }
            memset(grp, 0, sizeof(config_group_t));
            memcpy(grp->_grp_name, ptr + 1, len - 2);
            grp->_grp_name[len - 2] = '\0';
            if( cur != cfg->_grps ) {
                cur->_sibling = grp;
            } else {
                cur->_child = grp;
            }
            cur = grp;
            continue;
        }
        if( cur->_item_count >= MAX_CONFIG_GROUP_SIZE ) {
            fprintf(stderr, 
                    "config [ERROR] too many elements in group '%s', max is %d.\n", 
                    cur->_grp_name, MAX_CONFIG_GROUP_SIZE);
            fclose(fp);
            destroyConfig(cfg);
            return NULL;
        }
        if( *ptr == '=' ) {
            fprintf(stderr, "config [WARN] empty key at line %d\n", line);
            continue;
        }
        equal = strchr(ptr, '=');
        if( !equal ) {
            if( len > MAX_CONFIG_VAL_LEN ) {
                fprintf(stderr, "config [WARN] element is too long at line %d\n", line);
                continue;
            }
            strcpy(cur->_items[cur->_item_count]._val, ptr);
            cur->_items[cur->_item_count]._pair = false;
            cur->_item_count++;
        } else {
            *equal = '\0';
            p = equal - 1;
            while( isspace(*p) || *p == '\n' || *p == '\r' ) {
                *p = '\0';
                p--;
            }
            if( p - ptr > MAX_CONFIG_KEY_LEN ) {
                fprintf(stderr, "config [WARN] key is too long at line %d\n", line);
                continue;
            }
            equal++;
            while( isspace(*equal) || *equal == '\n' || *equal == '\r' ) {
                equal++;
            }
            if( !(*equal) ) {
                fprintf(stderr, "config [WARN] empty value at line %d\n", line);
                continue;
            }
            if( len - (equal - ptr) > MAX_CONFIG_VAL_LEN ) {
                fprintf(stderr, "config [WARN] value is too long at line %d\n", line);
                continue;
            }
            strcpy(cur->_items[cur->_item_count]._key, ptr);
            strcpy(cur->_items[cur->_item_count]._val, equal);
            cur->_items[cur->_item_count]._pair = true;
            h = hashString32(cur->_items[cur->_item_count]._key) % MAX_CONFIG_GROUP_SIZE_PRIME;
            cur->_item_count++;
            while( 1 ) {
                if( cur->_mapper[h] == 0 ) {
                    cur->_mapper[h] = cur->_item_count;
                    break;
                }
                h++;
                if( h >= MAX_CONFIG_GROUP_SIZE_PRIME ) {
                    h = 0;
                }
            }
        }
    }
    fclose(fp);
    return cfg;
}

static void destroyConfigGroup(config_group_t * grp) {
    if( !grp ) {
        return;
    }
    destroyConfigGroup(grp->_child);
    destroyConfigGroup(grp->_sibling);
    free(grp);
    return;
}

void destroyConfig(config_t * cfg) {
    if( !cfg ) {
        return;
    }
    destroyConfigGroup(cfg->_grps);
    free(cfg);
    return;
}

config_group_t * findConfigGroup(config_t * cfg, const char * name) {
    int nlen;
    config_group_t * grp;
    char str[MAX_CONFIG_KEY_LEN + 1];
    if( cfg == NULL ) {
        return NULL;
    }
    if( name == NULL || name[0] == '\0' ) {
        return cfg->_grps;
    }
    grp = cfg->_grps;
    while( grp ) {
        nlen = 0;
        while( name[nlen] != '\0' && name[nlen] != '/' ) {
            nlen++;
        }
        if( nlen == 0 ) {
            name++;
            continue;
        }
        memcpy(str, name, nlen);
        str[nlen] = '\0';
        grp = findChildConfigGroup(grp, str);
        if( name[nlen] == '\0' ) break;
        name += nlen + 1;
    }
    return grp;
}

config_group_t * findChildConfigGroup(config_group_t * p, const char * name) {
    if( p == NULL || p->_child == NULL || name == NULL ) {
        return NULL;
    }
    return findSiblingConfigGroup(p->_child, name);
}

config_group_t * findSiblingConfigGroup(config_group_t * grp, const char * name) {
    while( grp ) {
        if( strcasecmp(grp->_grp_name, name) == 0 ) {
            break;
        }
        grp = grp->_sibling;
    }
    return grp;
}

std::string getConfigPathValue(config_group_t * grp,
        const char * key, const std::string& root)
{

    std::string oriValue = getConfigValue(grp, key);
    if (!root.empty() && !oriValue.empty() && *(oriValue.begin()) != '/') {
        return joinFilePath(root, oriValue);
    }
    return oriValue;
}

const char * getConfigValue(config_group_t * grp, const char * key) {
    uint32_t h;
    if( grp == NULL || key == NULL || key[0] == '\0' ) {
        return "";
    }
    h = hashString32(key) % MAX_CONFIG_GROUP_SIZE_PRIME;
    while( grp->_mapper[h] != 0 ) {
        if( strcmp(grp->_items[grp->_mapper[h] - 1]._key, key) == 0 ) {
            return grp->_items[grp->_mapper[h] - 1]._val;
        }
        h++;
        if( h >= MAX_CONFIG_GROUP_SIZE_PRIME ) {
            h = 0;
        }
    }
    return "";
}

config_group_t * getRootConfigGroup(config_t * cfg) {
    return cfg ? cfg->_grps : NULL;
}

void firstConfigItem(config_group_t * grp) {
    if( grp ) {
        grp->_cursor = 0;
    }
}

bool nextConfigItem(config_group_t * grp, const char ** key, const char ** val) {
    if( !grp || grp->_cursor >= grp->_item_count ) {
        return false;
    }
    if( key ) {
        if( grp->_items[grp->_cursor]._pair ) {
            *key = grp->_items[grp->_cursor]._key;
        } else {
            *key = NULL;
        }
    }
    if( val ) {
        *val = grp->_items[grp->_cursor]._val;
    }
    grp->_cursor++;
    return true;
}

void displayConfig(config_t * cfg) {
    if( !cfg ) {
        return;
    }
    displayConfigGroup(cfg->_grps, "");
}

void displayConfigGroup(config_group_t * grp, const char * surfix) {
    uint32_t i;
    char buf[256];
    if( !grp ) {
        return;
    }
    if( surfix == NULL ) {
        surfix = "";
    }
    fprintf(stdout, "%s==> %s <==\n", surfix, grp->_grp_name);
    for( i = 0; i < grp->_item_count; i++ ) {
        if( grp->_items[i]._pair ) {
            fprintf(stdout, "%s[%s = %s]\n", surfix, grp->_items[i]._key, grp->_items[i]._val);
        } else {
            fprintf(stdout, "%s[%s]\n", surfix, grp->_items[i]._val);
        }
    }
    snprintf(buf, 128, "%s  ", surfix);
    displayConfigGroup(grp->_child, buf);
    displayConfigGroup(grp->_sibling, surfix);
}

config_group_t * getSiblingConfigGroup(config_group_t * p) {
    return p ? p->_sibling : NULL;
}

config_group_t * getChildConfigGroup(config_group_t * p) {
    return p ? p->_child : NULL;
}

}
