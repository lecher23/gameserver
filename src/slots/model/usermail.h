#ifndef USERMAIL_H
#define USERMAIL_H

#include <util/common_define.h>
#include <slots/model/mailinfo.h>

BEGIN_NAMESPACE(slots)

struct UserMail{
        SlotsMail mailInfo;
        bool bRead;
        bool bGet;
        bool bDel;
};
DF_SHARED_PTR(UserMail);

END_NAMESPACE
#endif
