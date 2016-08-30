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

    PayChecker(int32_t poolTaskSize, int32_t poolSize,
               int32_t queueSize, int32_t expireTime);
    ~PayChecker();

    void setSaveFunc(SaveFunc f);

    bool start();
    bool stop();
    bool doCheck(PayInfo *info);// only net error return false
    void clearExpireData();
    const PayInfo &getPayData(const std::string &id);

private:
    ThreadPool _pool;
    CircularQueue<PayInfo> _payInfos;
    int32_t _expireTime;

    SaveFunc _saveFunc;
    std::mutex _lock;
};

END_NAMESPACE
#endif
