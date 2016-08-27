#ifndef __UTIL_TIME_UTIL_H_
#define __util_TIME_UTIL_H_

#include <sys/time.h>
#include <time.h>
#include <string>
#include <stdint.h>

namespace cgserver
{

class CTimeUtil
{
public:
    static int64_t getCurrentTime();
    inline static int64_t getCurrentTimeInSeconds()
    {
        return getCurrentTime() / 1000000;
    }
    inline static int64_t getCurrentTimeInMillSeconds()
    {
        return getCurrentTime() / 1000;
    }
    inline static int64_t getCurrentTimeInMicroSeconds()
    {
        return getCurrentTime();
    }
    inline static int64_t getMicroSeconds(int64_t usecOffset = 0)
    {
        return getCurrentTime() + usecOffset;
    }
    static time_t getTime();

    static timeval getTimeval();
    static timeval getTimeval(int64_t usecOffset);
    static timespec getTimespec(int64_t usecOffset = 0);
    static std::string getCurrentTimeString();
     /**
     * Converts the time to a {struct tm} (static version).
     * This method corrects for the local time zone.
     * If timer represents a date before midnight, January 1, 1970,
     * localtime_r returns NULL.
     * @param  clock        time_t value.
     * @param  tmspace      Pointer to struct tm.
     * @return              Pointer to tm structure, or NULL on failure.
     */
    inline static tm *localtime_r(time_t *timeVal, tm *tmspace)
    {
        return ::localtime_r(timeVal, tmspace);
    }

    /**
     * Converts the time to a {struct tm} (static version).
     * The fields of the returned structure hold the evaluated value
     * of the timer argument in UTC rather than in local time.
     * If timer represents a date before midnight, January 1, 1970,
     * gmtime returns NULL. 
     * Note: @ref gmtime_r() is thread safe, @ref gmtime() is not.
     * @param  clock        time_t value.
     * @param  tmspace      Pointer to struct tm.
     * @return              Pointer to struct tm, or NULL on failure.
     */
    static tm *gmtime_r(time_t *clock, tm *tmspace);

    inline static tm *gmtime_r( tm * timeVal)
    {
        time_t now=getTime();
        return  gmtime_r(&now, timeVal);
    }
    
    /**
     * Parse a GMT date string.
     * Examples:
     * "Thu Aug 24 13:11:29 GMT 2000", 
     * "May 30 07:00:01 1976", 
     * "Mon 07:00:01 May 30 1976".
     * Only the following parts of the tm structure is filled in:
     * <PRE>
     *   int tm_sec     seconds after the minute - [0,59]
     *   int tm_min     minutes after the hour - [0,59]
     *   int tm_hour    hours since midnight - [0,23]
     *   int tm_mday    day of the month - [1,31]
     *   int tm_mon     months since January - [0,11]
     *   int tm_year    years since 1900
     * </PRE>
     * @param  date         Date string
     * @param  roundup      1: Round numbers up, 0: no round
     * @param  result       Pointer to struct tm
     * @return              0 on success, -1 on failure.
     */
    static int parseGMTDate(const char *date, int roundup, tm *result);

    /**
     * Another variant of ParseGMTDate. This version takes a date
     * string as input parameter and returns a time_t value.
     * @param  date         Date string
     * @return              time_t value, or -1 on failue.
     */
    static time_t parseGMTDate(const char *date);

    /**
     * Produce a GMT string of the supplied time_t value.
     * @param  buf          Output string buffer
     * @param  timeVal      Time value
     * @return              Pointer to string buffer
     */          
    static char *toGMTTimeString(char buf[30], time_t timeVal);

    /**
     * Produce a log string of the getCurrent time/date.
     * @param  buf          Output string buffer
     *                      (need at least 30 chars)
     * @return              Pointer to string buffer
     */          
    static char *logLocalTimeString(char *buf);


    
};

}

#endif

