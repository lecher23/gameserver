#include "paychecker.h"

BEGIN_NAMESPACE(cgserver)

PayChecker::PayChecker(int32_t poolTaskSize, int32_t poolSize,
                       int32_t queueSize, int32_t expireTime)
:_pool(poolSize, poolTaskSize),_payInfos(queueSize),_expireTime(expireTime)
{
    // init thread pool
}

PayChecker::~PayChecker(){
}

void PayChecker::setSaveFunc(SaveFunc f){
    _saveFunc = f;
}

bool PayChecker::start() {
    CLOG(INFO) << "Start pay checker." ;
    return _pool.start();
}

bool PayChecker::stop() {
    _pool.stop();
    // TODO: save info to db and log file
}

bool PayChecker::doCheck(PayInfo *info) {
    if (info == nullptr){
        return true;
    }
    info->status = EPS_CHECKING;
    // TODO: request remote server.

    if (info->status == EPS_NET_ERROR) {
        return false;
    }
    return true;
}

void PayChecker::clearExpireData() {
    MUTEX_GUARD(_lock);
    int64_t filterTime = CTimeUtil::getCurrentTimeInSeconds() - _expireTime;
    while(!_payInfos.empty()) {
        auto &top = _payInfos.front();
        if(top.timestamp > filterTime){
            break;
        }
        if (!_saveFunc(&top)){
            CLOG(WARNING) << "Save pay info failed, pay id:" << top.id;
            break;
        }
        _payInfos.pop_front();
    }
}

const PayInfo &PayChecker::getPayData(const std::string &id){
    PayInfo *pi = _payInfos.find([&](PayInfo *src){return src->id == id;});
    if (pi == nullptr) {
        MUTEX_GUARD(_lock);
        _payInfos.push_back(PayInfo(id));
        return _payInfos.back();
    }
    return *pi;
}

END_NAMESPACE
