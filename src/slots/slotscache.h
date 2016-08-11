#ifndef SLOTS_CACHE_H  
#define SLOTS_CACHE_H  
  
#include <stdlib.h>  
#include <iostream>  
#include <map>  
#include <time.h>  
#include <pthread.h>
#include "../util/common_define.h"
#include "slotsdb.h"

using namespace std;
namespace slots{
    template<class T1, class T2>  
    class SlotsCache  
    {  
    public:  
        struct cache_data  
        {  
            time_t begin_time;  
            time_t update_time;  
            int expire_time;  
            int count;  
            T2 data;  
        };

	enum THREAD_TYPE {
	    DUMP_THREAD_RUNNING,
	    DUMP_THREAD_STOPPED
	};
  
        SlotsCache(){
	    m_max_size = 0;  
	    m_avg_expire_time = 0;  
	    m_mutex = NULL;  
	    m_cache_map = NULL;
	    _stop = false;
	    _dump_thread_status = DUMP_THREAD_STOPPED;
	}  
  
        virtual ~SlotsCache()  
	    {  
		m_max_size = 0;  
		if(NULL != m_cache_map)  
		    {  
			delete m_cache_map;  
			m_cache_map = NULL;  
		    }  
  
		if(m_mutex != NULL)  
		    {  
			pthread_mutex_destroy(m_mutex);  
		    }  
	    }  

	// may failed.
        bool init(int max_cache_size, bool need_dump_thread, int dump_interval_sec)  
        {  
            m_mutex = new pthread_mutex_t;  
            m_cache_map = new map<T1, cache_data>;  
            m_max_size = max_cache_size;
	    dump_interval = dump_interval_sec;
            pthread_t id;  
            pthread_mutex_init(m_mutex, NULL);
	    if (!need_dump_thread) {
		return true;
	    }
            int ret = pthread_create(&id, NULL, dump_thread, this);
	    if (ret != 0) {
		CLOG(ERROR) << "Create cache expire thread failed:"<< ret;
		return false;
	    }
            return true;  
        }  
  
        bool get(const T1& key, T2& data)  
        {  
            pthread_mutex_lock(m_mutex);  
            typename map<T1, cache_data>::iterator it = m_cache_map->find(key);  
            if(it == m_cache_map->end())  
		{  
		    pthread_mutex_unlock(m_mutex);  
		    return false;  
		}  
	    // will cause copy constructor
            data = it->second.data;  
            it->second.count++;  
            it->second.update_time = time(NULL);  
            pthread_mutex_unlock(m_mutex);  
            return true;  
        }  
  
        bool set(const T1& key, const T2& data, const int& expire_time = -1)  
        {  
            cache_data new_data;  
            new_data.count = 0;  
            new_data.expire_time = expire_time;  
            new_data.begin_time = time(NULL);  
            new_data.update_time = new_data.begin_time;  
            new_data.data = data;  
            pthread_mutex_lock(m_mutex);  
            if(m_cache_map->size() >= m_max_size)  
		{  
		    pthread_mutex_unlock(m_mutex);  
		    lru_remove();  
		    pthread_mutex_lock(m_mutex);  
		}  
            m_cache_map->insert(pair<T1, cache_data>(key, new_data));  
            pthread_mutex_unlock(m_mutex);  
            return true;  
        }  
  
        bool remove(const T1& key)  
        {  
            pthread_mutex_lock(m_mutex);  
            if(m_cache_map->find(key) != m_cache_map->end())  
		{  
		    m_cache_map->erase(key);  
		    pthread_mutex_unlock(m_mutex);  
		    return true;  
		}  
  
            pthread_mutex_unlock(m_mutex);  
            return false;  
        }  
  
        bool lru_remove()  
        {  
            pthread_mutex_lock(m_mutex);  
            typename map<T1, cache_data>::iterator lru_it = m_cache_map->begin();  
            typename map<T1, cache_data>::iterator it = m_cache_map->begin();  
            if(it == m_cache_map->end())  
		{  
		    pthread_mutex_unlock(m_mutex);  
		    return false;  
		}  
  
            size_t weight = it->second.count + it->second.update_time;  
            for(++it; it != m_cache_map->end(); ++it)  
		{  
		    if(it->second.count + it->second.update_time < weight)//lru  
			{  
			    weight = it->second.count + it->second.update_time;  
			    lru_it = it;  
			}  
		}  
            m_cache_map->erase(lru_it);  
            pthread_mutex_unlock(m_mutex);  
            return true;  
        }  
  
        int lru_expire()  
        {  
            int count = 0;  
            pthread_mutex_lock(m_mutex);  
            long long sum = 0;  
            for(typename map<T1, cache_data>::iterator it = m_cache_map->begin();  
                it != m_cache_map->end();)  
		{  
		    sum += it->second.expire_time;  
		    if(it->second.expire_time >= 0  
		       && time(NULL) - it->second.update_time >= it->second.expire_time)  
			{  
			    m_cache_map->erase(it++);  
			    ++count;  
			}  
		    else  
			{  
			    ++it;  
			}  
		}  
  
            if(!m_cache_map->empty())  
		{  
		    m_avg_expire_time = sum / m_cache_map->size();  
		}  
            pthread_mutex_unlock(m_mutex);  
  
            return count;  
        }  
  
        int get_avg_expire_time()const  
        {  
            pthread_mutex_lock(m_mutex);  
            int avg_expire_time = m_avg_expire_time;  
            pthread_mutex_unlock(m_mutex);  
            return avg_expire_time;  
        }

	void write_to_db() {
            pthread_mutex_lock(m_mutex);
	    CLOG(INFO) << "dump data to db.";
	    SlotsDB &db = SlotsDB::getInstance();
	    typename map<T1, cache_data>::const_iterator itr
		= m_cache_map->begin();
	    for (; itr != m_cache_map->end(); ++itr) {
		db.update((itr->second).data);
	    }
	    CLOG(INFO) << "dump data to db finish.";	    
            pthread_mutex_unlock(m_mutex);  	    
	}

	void stop_cache() {
	    _stop = true;
	    // wait until cache stop;
	    while (_dump_thread_status != DUMP_THREAD_STOPPED) {
		sleep(1);
	    }
	    // write data to mysql
	    write_to_db();
	}
	
    protected:  
        static void* dump_thread(void *args)  
        {
            SlotsCache *p_instance = (SlotsCache *)args;
	    p_instance->_dump_thread_status = DUMP_THREAD_RUNNING;
            while(!p_instance->_stop){
		sleep(p_instance->dump_interval);  
		p_instance->write_to_db();  
	    }
	    p_instance->_dump_thread_status = DUMP_THREAD_STOPPED;
            return NULL;  
        }
  
    protected:  
        int m_max_size;  
        int m_avg_expire_time;
	int dump_interval; // second
        map<T1, cache_data> *m_cache_map;  
        pthread_mutex_t *m_mutex;
	
	bool _stop;
	THREAD_TYPE _dump_thread_status;
    };
}  
#endif  

