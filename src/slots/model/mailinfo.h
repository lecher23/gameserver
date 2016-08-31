#ifndef MAILINFO_H
#define MAILINFO_H

#include <util/common_define.h>

BEGIN_NAMESPACE(slots)

struct MailInfo{
    std::string mailId;
    std::string title;
    std::string content;
    int32_t attachment;
    std::string createTime;
    int32_t keepDays;
};
DF_SHARED_PTR(MailInfo);

#define SlotsMail MailInfo
#define SlotsMailPtr MailInfoPtr

struct Attachment{
    int32_t type;
    int64_t value;
};
DF_SHARED_PTR(Attachment);

END_NAMESPACE
#endif
