#ifndef SLOTSDATACENTER_H
#define SLOTSDATACENTER_H

#include "slotscache.h"
#include "slotsdb.h"

namespace slots{
    /*This lru cache will never expired*/
    class SlotsDataCenter{
    public:
	
	~SlotsDataCenter(){
	}

	bool init (bool needDump = true, int dumpInterval = 60){
	    _suCache.init(300, needDump, 60);
	}

	static SlotsDataCenter &instance(){
	    static SlotsDataCenter cache;
	    return cache;
	}

	bool get(std::string &uid, SlotsUserPtr &out) {
	    bool ret = _suCache.get(uid, out);
	    if (!ret) {
		CLOG(INFO) << "Get user["<< uid <<"] info from db.";
		SlotsDB &db = SlotsDB::getInstance();
		out.reset(new SlotsUser);
		std::string err;
		ret = db.getUserInfoByUserId(uid, *out);
		if (ret) {
		    out->uInfo.changed = false;
		    out->uRes.changed = false;
		    set(out);
		} else {
		    CLOG(WARNING) << "Get user[" << uid << "] info failed.";
		}
	    }else {
		ret = (out.get() != NULL);
		CLOG(INFO) << "Get user["<< uid <<"] info from cache.";
	    }
	    return ret;
	}

	bool set(SlotsUserPtr &input) {
	    return _suCache.set(input->uInfo.uid, input);
	}

    private:
	SlotsDataCenter(){}
	SlotsDataCenter(const SlotsDataCenter &);
	
	SlotsCache<std::string, SlotsUserPtr> _suCache;
    };
}
#endif
