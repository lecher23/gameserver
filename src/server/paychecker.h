#ifndef PAYCHECKER_H
#define PAYCHECKER_H

#include <util/circularqueue.h>
#include <util/common_define.h>
#include <util/timeutil.h>
#include <util/threadpool.h>

BEGIN_NAMESPACE(cgserver)

enum PayStatus{
    EPS_INVALID,
    EPS_UNKNOW,
    EPS_CHECKING,
    EPS_OK,
    EPS_FAIL,
    EPS_NET_ERROR
};

struct PayInfo{
    std::string id;
    PayStatus status;
    int64_t timestamp;
    int32_t retryTimes;
    PayInfo(const std::string &id_):id(id_) {
        status = EPS_UNKNOW;
        timestamp = CTimeUtil::getCurrentTimeInSeconds();
        retryTimes = 0;
    }
    PayInfo(){status = EPS_INVALID;}
};

class PayChecker{
 public:
    typedef std::function<bool(PayInfo *)> SaveFunc;
    typedef std::function<void(asio_service *, PayInfo *)> AsyncCheckFunc;

    PayChecker(int32_t queueSize, int32_t expireTime);
    ~PayChecker();

    void setSaveFunc(SaveFunc f);
    void setAsyncCheckFunc(AsyncCheckFunc f);
    void threadFunc();

    bool start();
    bool stop();
    void asyncCheck(PayInfo &info);// only net error return false
        const PayInfo &getPayData(const std::string &id);

private:
    void clearExpireData(const asio_error &err);
    CircularQueue<PayInfo> _payInfos;
    int32_t _expireTime;

    asio_service _service;
    std::shared_ptr<std::thread> _sThread;
    std::shared_ptr<asio_service::work> _work;
    asio_deadline_timer_ptr _timer;
    AsyncCheckFunc queryRemoteServer;

    SaveFunc _saveFunc;
    std::mutex _lock;
};

END_NAMESPACE
#endif
