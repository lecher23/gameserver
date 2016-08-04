#ifndef SLOTSLRUCACHE_H
#define SLOTSLRUCACHE_H

#include "../util/lrucache.h"
#include "slotsdb.h"

namespace slots{
    /*This lru cache will never expired*/
    class SlotsLruCache{
    public:
	
	~SlotsLruCache(){
	}

	bool init (){
	    _suCache.init(300, false);
	}

	static SlotsLruCache &instance(){
	    static SlotsLruCache cache;
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
	SlotsLruCache(){}
	SlotsLruCache(const SlotsLruCache &);
	
	cgserver::LruCache<std::string, SlotsUserPtr> _suCache;
    };
}
#endif
