#ifndef CARGOHISTORY_H
#define CARGOHISTORY_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct CargoHistory{
    std::string uid; // use id
    std::string cid; // cargo id
    std::string tsid; // transation id
    int64_t value; // final value at the end of transaction
    int64_t timestamp; // unix timestamp
};

END_NAMESPACE
#endif
