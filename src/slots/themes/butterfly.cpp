#include "butterfly.h"

#include <util/random.h>

BEGIN_NAMESPACE(slots)

Butterfly::Butterfly(){
}

Butterfly::~Butterfly(){
}

void Butterfly::play(
    int32_t selectCount, int32_t &multiple, std::vector<int32_t> &ret) const
{
    std::vector<int32_t> helper(factors.size());
    for (size_t i = 0; i < factors.size(); ++i) {
        helper[i] = i;
    }
    multiple = 0;
    ret.clear();
    cgserver::Random::getInstance().randMutiVal(selectCount, helper);

    for (size_t i = 0; i < selectCount; ++i) {
        if (helper[i] == 0) {
            multiple = factors[helper[i]];
        }else{
            ret.push_back(factors[helper[i]]);
        }
    }
}

void Butterfly::addFactor(int32_t val) {
    factors.push_back(val);
}

END_NAMESPACE
