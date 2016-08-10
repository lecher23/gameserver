#ifndef GIFTSDATA_H
#define GIFTSDATA_H

#include "../util/common_define.h"
#include "../util/timeutil.h"

namespace slots{
    #define MAX_GIFT_COUNT 30
    #define GIFT_SENT_LIMIT 30

    struct GiftRecord{
	uint64_t master;
	uint64_t guest;
	int64_t timestamp;
	bool received;
    GiftRecord(uint64_t m, uint64_t g):master(m), guest(g),
	    timestamp(cgserver::CTimeUtil::getCurrentTimeInSeconds()),received(false){}
    };
    DF_SHARED_PTR(GiftRecord);
    typedef std::vector<GiftRecordPtr> Gifts;
    
    class GiftsData{
    public:
        GiftsData(int64_t val):_giftValue(val){}
        ~GiftsData(){}

	bool init(){
            int ret = pthread_create(&_tid, NULL, monitor, this);
	    if (ret != 0) {
		CLOG(ERROR) << "Create gift monitor thread failed:"<< ret;
		return false;
	    }
            return true;  
	}

	bool sendGift(uint64_t master, uint64_t guest) {
	    if (getSendGiftCount(master) > GIFT_SENT_LIMIT) {
		//CLOG(WARNING) << "Send gift exceed limit";
		return false;
	    }
	    if (getReceiveGiftCount(guest) > MAX_GIFT_COUNT) {
		//CLOG(WARNING) << "receive gift exceed limit";		
		return false;
	    }
	    if (hasSendGift(master, guest)) {
		//CLOG(WARNING) << "already send gift";				
		return false;
	    }
	    GiftRecordPtr tmp(new GiftRecord(master, guest));
	    MUTEX_GUARD(_lock);
	    _sendLimit[master] ++;
	    _receiveLimit[guest] ++;
	    _giftsData.push_back(tmp);
	    return true;
	}

	void getGifts(uint64_t guest, Gifts &out) {
	    MUTEX_GUARD(_lock);
	    for (auto val: _giftsData) {
		if (val->guest == guest) {
		    out.push_back(val);
		}
	    }
	}

	// taker take gift from tar
	bool takeGift(uint64_t taker, uint64_t tar) {
	    MUTEX_GUARD(_lock);
	    for (auto val: _giftsData) {
		if (val->master == tar && val->guest==taker && !val->received) {
		    val->received = true;
		    return true;
		}
	    }
	    return false;
	}

	int64_t getGiftValue() {
	    return _giftValue;
	}

	int64_t getGiftSum(uint64_t uid) {
	    int64_t ret = 0;
	    MUTEX_GUARD(_lock);	    
	    for (auto val: _giftsData) {
		if (val->guest==uid && !val->received) {
		    val->received = true;
		    ret += _giftValue;
		}
	    }
	    return ret;
	}

    private:
	static void *monitor(void *args) {
	    GiftsData *gd = (GiftsData *) args;
	    while(true) {
		CLOG(INFO) << "Clear gift information.";
		gd->_lock.lock();
		gd->_sendLimit.clear();
		gd->_receiveLimit.clear();
		gd->_giftsData.clear();
		gd->_lock.unlock();
		sleep(24*60*60);
	    }
	}
	
	bool hasSendGift(uint64_t m, uint64_t g) {
	    for (auto val: _giftsData) {
		if (val->master == m && val->guest==g) {
		    return true;
		}
	    }
	    return false;
	}
	
	uint32_t getSendGiftCount(uint64_t idx) {
	    auto itr = _sendLimit.find(idx);
	    if (itr == _sendLimit.end()){
		_sendLimit[idx] = 0;
		return 0;
	    }
	    return itr->second;
	}

	uint32_t getReceiveGiftCount(uint64_t idx) {
	    auto itr = _receiveLimit.find(idx);
	    if (itr == _receiveLimit.end()){
		_receiveLimit[idx] = 0;
		return 0;
	    }
	    return itr->second;
	}
	
	std::map<uint64_t, uint32_t> _sendLimit;
	std::map<uint64_t, uint32_t> _receiveLimit;
	Gifts _giftsData;
	int64_t _giftValue;
	std::mutex _lock;
	pthread_t _tid;
    };
    DF_SHARED_PTR(GiftsData);
}
#endif
