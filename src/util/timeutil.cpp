#include <string.h>
#include <stdio.h>
#include "timeutil.h"

namespace cgserver
{
    
int64_t CTimeUtil::getCurrentTime()
{
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return (tval.tv_sec * 1000000LL + tval.tv_usec);
}

int64_t CTimeUtil::getMorningTime() {
    time_t t = time(0);
    struct tm *zeroTm = localtime(&t);
    zeroTm->tm_hour = 0;
    zeroTm->tm_min = 0;
    zeroTm->tm_sec = 0;
    return mktime(zeroTm);
}

inline time_t CTimeUtil::getTime()
{
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return static_cast<time_t>(tval.tv_sec);
}

timeval CTimeUtil::getTimeval()
{
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return tval;
}
timeval CTimeUtil::getTimeval(int64_t usecOffset)
{
    timeval tval;
    int64_t uTime = getMicroSeconds(usecOffset);
    tval.tv_sec = uTime / 1000000;
    tval.tv_usec = uTime % 1000000;
    return tval;
}

timespec CTimeUtil::getTimespec(int64_t usecOffset)
{
    timespec tspec;
    int64_t uTime = getMicroSeconds(usecOffset);
    tspec.tv_sec = uTime / 1000000;
    tspec.tv_nsec = (uTime % 1000000) * 1000;
    return tspec;
}

std::string CTimeUtil::getCurrentTimeString()
{
    time_t lt = time(NULL);
    struct tm *ptr = localtime(&lt);
    char str[200];
    strftime(str, sizeof(str), "%Y%m%d%H%M%S", ptr);
    return std::string(str);
}


tm *CTimeUtil::gmtime_r(time_t *gmt, tm *tmspace)
{
    static int monadd2[2][12]= { {0, 31, 59, 90, 120, 151,
                                  181, 212, 243, 273, 304, 334},
                                 {0, 31, 60, 91, 121, 152,
                                  182, 213, 244, 274, 305, 335} };

    int leapyear;
    int year;
    int days;
    int mon;
    int day;
    int hour;
    int minute;
    int subday;
    int mday;
    int *monadd;

    day = *gmt / (24 * 60 * 60);
    subday = *gmt - day * (24 * 60 * 60);
    if (*gmt < 0 && subday < 0) {
        day--;
        subday += (24 * 60 * 60);
    }

    hour = subday / (60 * 60);
    subday -= hour * 60 * 60;
    minute = subday / 60;

    tmspace->tm_sec = subday - minute * 60;
    tmspace->tm_min = minute;
    tmspace->tm_hour = hour;

    year = (day + 719067) / 365;

    for (;;) 
    {
        if ((year % 400) == 0)
            leapyear = 1;
        else if ((year % 100) == 0)
            leapyear = 0;
        else if (year % 4)
            leapyear = 0;
        else
            leapyear = 1;
        days = year * 365 + (year / 4) - (year / 100) + (year / 400) +
               1 - 719528 - leapyear;
        if (day >= days)
            break;
        year--;
    }
    monadd = monadd2[leapyear];

    if (day >= days + monadd[6])
        mon = 6;
    else
        mon = 0;
    if (day >= days + monadd[mon + 3])
        mon+= 3;
    if (day >= days + monadd[mon + 2])
        mon+= 2;
    else if (day >= days + monadd[mon + 1])
        mon++;

    mday = day - days - monadd[mon];

    tmspace->tm_mday = mday + 1;
    tmspace->tm_mon = mon;
    tmspace->tm_year = year - 1900;
    tmspace->tm_wday = (day + 24861) % 7;
#ifdef notdef
    tmspace->tm_yday = day - days;
    tmspace->tm_isdst = 0;
#ifdef __FreeBSD__
    static const gmtstr[4] = "GMT";
    tmspace->tm_zone = gmtstr;
    tmspace->tm_gmtoff = 0;
#endif
#endif

    return tmspace;
}

    
int CTimeUtil::parseGMTDate(const char *date, int roundup,tm *result)
{
    static const char *monname[13] = {"xxx", "jan", "feb", "mar",
                                      "apr", "may", "jun", "jul",
                                      "aug", "sep", "oct", "nov",
                                      "dec" };
    char tmpmon[16];
    const char *p;
    char *q;
    unsigned int month, year, day, hour, minute, sec, leapyear;
    unsigned int num, num1;
    char num1_given;
    char year_given;
    char month_given;
    char day_given;
    char hour_given;
    char minute_given;
    char sec_given;
    int error = 0;

    num1_given = 0;
    num1 = 0;
    year_given = 0;
    month_given = 0;
    day_given = 0;
    hour_given = 0;
    minute_given = 0;
    sec_given = 0;

    hour = 0;
    minute = 0;
    sec = 0;
    day = 1;
    month = 1;
    year = 1970;

    p = date;
    while (*p == ' ')
        p++;
    if (*p == '\0')
        return -1;

    /* Skip weekday */
    if (((p[0] == 'M' || p[0] == 'm') &&
         (p[1] == 'O' || p[1] == 'o') &&
         (p[2] == 'N' || p[2] == 'n')) ||
        ((p[0] == 'T' || p[0] == 't') &&
         (p[1] == 'U' || p[1] == 'u') &&
         (p[2] == 'E' || p[2] == 'e')) ||
        ((p[0] == 'W' || p[0] == 'w') &&
         (p[1] == 'E' || p[1] == 'e') &&
         (p[2] == 'D' || p[2] == 'd')) ||
        ((p[0] == 'T' || p[0] == 't') &&
         (p[1] == 'H' || p[1] == 'h') &&
         (p[2] == 'U' || p[2] == 'u')) ||
        ((p[0] == 'F' || p[0] == 'f') &&
         (p[1] == 'R' || p[1] == 'r') &&
         (p[2] == 'I' || p[2] == 'i')) ||
        ((p[0] == 'S' || p[0] == 's') &&
         (p[1] == 'A' || p[1] == 'a') &&
         (p[2] == 'T' || p[2] == 't')) ||
        ((p[0] == 'S' || p[0] == 's') &&
         (p[1] == 'U' || p[1] == 'u') &&
         (p[2] == 'N' || p[2] == 'n'))) 
    {
        p += 3;
        while ((*p >= 'a' && *p <= 'z') ||
               (*p >= 'A' && *p <= 'Z'))
            p++;
    }
    while (*p != '\0' && error == 0) 
    {
        if (*p <= '9' && *p >= '0') 
        {
            num = 0;
            while (*p <= '9' && *p >= '0') 
            {
                num = num * 10 + *p - '0';
                p++;
            }
            if (*p == ':' && hour_given == 0 && minute_given == 0) 
            {
                hour_given = 1;
                hour = num;
                p++;
                minute = 0;
                while (*p <= '9' && *p >= '0') 
                {
                    minute = minute * 10 + *p - '0';
                    p++;
                }
                minute_given = 1;
                if (*p == ':' && sec_given == 0) 
                {
                    p++;
                    sec = 0;
                    while (*p <= '9' && *p >= '0') {
                        sec = sec * 10 + *p - '0';
                        p++;
                    }
                    sec_given = 1;
                }
            } 
            else if (year_given == 0) 
            {
                if (month_given == 0) 
                {
                    if (num1_given != 0) 
                    {
                        year_given = 1;
                        year = num1;
                        month_given = 1;
                        month = num;
                        num1_given = 0;
                    } 
                    else 
                    {
                        num1 = num;
                        num1_given = 1;
                    }
                } 
                else if (day_given == 0) 
                {
                    if (num1_given != 0) 
                    {
                        if (num1 > 31 && num <= 31) 
                        {
                            year = num1;
                            day = num;
                        } 
                        else 
                        {
                            year = num;
                            day = num1;
                        }
                        day_given = 1;
                        year_given = 1;
                        num1_given = 0;
                    }
                    else if (num <= 31) 
                    {
                        num1 = num;
                        num1_given = 1;
                    }
                    else 
                    {
                        year = num;
                        year_given = 1;
                    }
                } 
                else if (num <= 31) 
                {
                    year = num;
                    year_given = 1;
                }
            } 
            else if (month_given == 0) 
            {
                month = num;
                month_given = 1;
            }
            else if (day_given == 0) 
            {
                day = num;
                day_given = 1;
            } 
            else if (hour_given == 0) 
            {
                hour = num;
                hour_given = 1;
            } 
            else if (minute_given == 0) 
            {
                minute = num;
                minute_given = 1;
            }
            else if (sec_given == 0) 
            {
                sec = num;
                sec_given = 1;
            } 
            else 
            {
                error = 1;
            }
        } 
        else if ((p[0] == 'G' || p[0] == 'g') &&
                   (p[1] == 'M' || p[1] == 'm') &&
                   (p[2] == 'T' || p[2] == 't') &&
                   (p[3] == '\0' || p[3] == ' ')) 
        {
            p += 3;
        } 
        else if (((*p <= 'z' && *p >= 'a') || (*p <= 'Z' && *p >= 'A')) &&
                   month_given == 0) 
        {
            /* Parse month name */
            q = tmpmon;
            while ((*p <= 'z' && *p >= 'a') || (*p <= 'Z' && *p >= 'A')) 
            {
                if (static_cast<size_t>((q - tmpmon) + 1) >= sizeof(tmpmon))
                    break;
                *q = *p++;
                if (*q <= 'Z' && *q >= 'A')
                    *q = *q - 'A' + 'a';
                q++;
            }
            *q = 0;
            for (month = 1;month < 13; month++)
                if (strncmp(tmpmon, monname[month], 3) == 0)
                    break;
            if (month >= 13)
                error = 1;
            else 
            {
                month_given = 1;
            }
        } 
        else if (*p == '/' || *p == '-' || *p == ' ' || *p == ',')
            p++;
        else
            error = 1;
    }
    if (error == 0 && num1_given != 0) 
    {
        if (day_given == 0&& num1 <= 31) 
        {
            day = num1;
            day_given = 1;
            num1_given = 0;
        } 
        else if (year_given == 0) 
        {
            year = num1;
            year_given = 1;
            num1_given = 0;
        } 
        else if (month_given == 0) 
        {
            month = num1;
            month_given = 1;
            num1_given = 0;
        } 
        else if (hour_given == 0) 
        {
            hour = num1;
            hour_given = 1;
            num1_given = 0;
        } 
        else if (minute_given == 0) 
        {
            minute = num1;
            minute_given = 1;
            num1_given = 0;
        } 
        else if (sec_given == 0) 
        {
            sec = num1;
            sec_given = 1;
            num1_given = 0;
        } else
            error = 1;
    }

    if (year_given != 0 && year < 1900)
        year += 1900;

    if (year_given == 0) 
    {
        CTimeUtil now;
        tm ltm;
        now.gmtime_r(&ltm);
        year = ltm.tm_year + 1900;
        year_given = 1;
    }

    if ((year % 400) == 0)
        leapyear = 1;
    else if ((year % 100) == 0)
        leapyear = 0;
    else if (year % 4)
        leapyear = 0;
    else
        leapyear = 1;

    if (roundup) 
    {
        if (month_given == 0)
            month = 12;
        if (day_given == 0) 
        {
            if (month == 2) 
            {
                day= 28 + leapyear;
            } 
            else if (month < 8) 
            {
                day= 30 + (month % 2);
            } 
            else 
            {
                day= 31 - (month % 2);
            }
        }
        if (hour_given == 0)
            hour = 23;
        if (minute_given == 0)
            minute = 59;
        if (sec_given == 0)
            sec = 59;
    }

    if (month < 1 || month > 12 ||
        day < 1 || day > 31 ||
        year < 1000 || year > 4000 ||
        hour > 23 || minute > 59 || sec > 59) 
    {
        error = 1;
    } 
    else 
    {
        if (month == 2) 
        {
            if (day > 28 + leapyear)
                error = 1;
        }
        else if (month < 8) 
        {
            if (day > 30 + (month % 2))
                error = 1;
        } 
        else 
        {
            if (day > 31 - (month % 2))
                error = 1;
        }
    }
    if (error == 0) 
    {
        memset(result, 0, sizeof(*result));
        result->tm_sec = sec;
        result->tm_min = minute;
        result->tm_hour = hour;
        result->tm_mday = day;
        result->tm_mon = month - 1 ;
        result->tm_year = year - 1900;
        result->tm_isdst = -1;
        return 0;
    }
    return -1;
}

time_t CTimeUtil::parseGMTDate(const char *value)
{
    tm mytm;
    static int monadd[12] = {0, 31, 59, 90, 120, 151,
                             181, 212, 243, 273, 304, 334};
    int leapyear;
    int year;
    int days;

    if (parseGMTDate(value, 0, &mytm)) 
    {
        return -1;
    }

    year = mytm.tm_year + 1900;
    if ((year % 400) == 0)
        leapyear = 1;
    else if ((year % 100) == 0)
        leapyear = 0;
    else if (year % 4)
        leapyear = 0;
    else
        leapyear = 1;

    days = year * 365 + (year / 4) - (year / 100) + (year / 400) +
           monadd[mytm.tm_mon] + mytm.tm_mday - 719528;
    if (leapyear && mytm.tm_mon < 2)
        days--;

    return static_cast<time_t>
        (((days * 24) + mytm.tm_hour) * 60 + mytm.tm_min) * 60 + mytm.tm_sec;
}

char *CTimeUtil::toGMTTimeString(char buf[30], time_t now)
{
    tm *ltm;
    tm tmspace;
    static const char *Mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

    static const char *Day[7] = {"Sun", "Mon", "Tue", "Wed",
                                 "Thu", "Fri", "Sat" };

    ltm = gmtime_r(&now, &tmspace);

    sprintf(buf, "%s, %02d %s %d %02d:%02d:%02d",
            Day[ltm->tm_wday],
            ltm->tm_mday,
            Mon[ltm->tm_mon],
            ltm->tm_year + 1900,
            ltm->tm_hour,
            ltm->tm_min,
            ltm->tm_sec);
    return buf;
}




char *CTimeUtil::logLocalTimeString(char *buf)
{
    char sign;
    static const char *Mon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    time_t now =getTime();
    struct tm  gtm;
    CTimeUtil::gmtime_r(&now, &gtm);
    struct tm  ltm;
    CTimeUtil::localtime_r(&now, &ltm);
    int  off = (ltm.tm_hour - gtm.tm_hour) * 60 + ltm.tm_min - gtm.tm_min;
    if (ltm.tm_year == gtm.tm_year) 
    {
        if (ltm.tm_mon == gtm.tm_mon) 
        {
            if (ltm.tm_mday < gtm.tm_mday)
                off -= 24 * 60;
            else if (ltm.tm_mday > gtm.tm_mday)
                off += 24 * 60;
        } 
        else if (ltm.tm_mon < gtm.tm_mon)
            off -= 24 * 60;
        else
            off += 24 * 60;
    } 
    else 
    {
        if (ltm.tm_year < gtm.tm_year)
            off -= 24 * 60;
        else if (ltm.tm_year > gtm.tm_year)
            off += 24 * 60;
    }
    if (off == 0) 
    {
        sprintf(buf, "[%02d/%s/%d:%02d:%02d:%02d +0000]",
                ltm.tm_mday,
                Mon[ltm.tm_mon],
                ltm.tm_year + 1900,
                ltm.tm_hour,
                ltm.tm_min,
                ltm.tm_sec);
        return buf;
    }
    if (off<0) 
    {
        sign = '-';
        off = -off;
    } 
    else 
    {
        sign = '+';
    }
    off = (off/60)*100 + (off%60);
    sprintf(buf, "[%02d/%s/%d:%02d:%02d:%02d %c%04d]",
            ltm.tm_mday,
            Mon[ltm.tm_mon],
            ltm.tm_year + 1900,
            ltm.tm_hour,
            ltm.tm_min,
            ltm.tm_sec,
            sign, off);
    return buf;
}

}
