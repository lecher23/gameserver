#include "paychecker.h"
#include <boost/bind.hpp>

BEGIN_NAMESPACE(cgserver)

PayChecker::PayChecker(int32_t queueSize, int32_t expireTime)
:_payInfos(queueSize),_expireTime(expireTime)
{
    // init thread pool
}

PayChecker::~PayChecker(){
}

void PayChecker::setSaveFunc(SaveFunc f) {
    _saveFunc = f;
}

void PayChecker::setAsyncCheckFunc(AsyncCheckFunc f) {
    queryRemoteServer = f;
}

void PayChecker::threadFunc() {
    CLOG(INFO) << ("aiso service thread for pay checker start.");
    _service.run();
    CLOG(INFO) << ("aiso service thread for pay checker end.");
}

bool PayChecker::start() {
    CLOG(INFO) << "Start pay checker." ;
    _work.reset(new asio_work(_service));
    _timer.reset(new asio_deadline_timer(_service, asio_seconds(_expireTime)));
    _timer->async_wait(
        boost::bind(&PayChecker::clearExpireData, this, asio_placeholders::error));
    _sThread.reset(new std::thread(std::bind(&PayChecker::threadFunc, this)));
    return true;
}

bool PayChecker::stop() {
    if (_sThread.get() != nullptr) {
        _work.reset();
        _service.stop();
        _sThread->join();
    }
    while(!_payInfos.empty()) {
        auto &top = _payInfos.front();
        if (!_saveFunc(&top)){
            CLOG(WARNING) << "Save pay info failed, pay id:" << top.id
                          << ", pay status:" << top.status;
        }
        _payInfos.pop_front();
    }
}

void PayChecker::asyncCheck(PayInfo &info) {
    info.status = EPS_CHECKING;
    queryRemoteServer(&_service, &info);
}

void PayChecker::clearExpireData(const asio_error &err) {
    if (err) {
        CLOG(WARNING) << "Asio timer error:" << err;
    } else {
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
    _timer->expires_at(_timer->expires_at() + asio_seconds(_expireTime));
    _timer->async_wait(
        boost::bind(&PayChecker::clearExpireData, this, asio_placeholders::error));
}

const PayInfo &PayChecker::getPayData(const std::string &id){
    PayInfo *pi = _payInfos.find([&](PayInfo *src){return src->id == id;});
    if (pi == nullptr) {
        CLOG(INFO) << "New pay data to check: " << id;
        MUTEX_GUARD(_lock);
        _payInfos.push_back(PayInfo(id));
        asyncCheck(_payInfos.back());
        return _payInfos.back();
    }
    return *pi;
}

END_NAMESPACE
