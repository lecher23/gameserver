#ifndef TIMEREXECUTOR_H
#define TIMEREXECUTOR_H

#include <util/common_define.h>

BEGIN_NAMESPACE(cgserver)

class TimerExecutor{
 public:
    TimerExecutor();
    virtual ~TimerExecutor();

    virtual bool execute() = 0;
};

DF_SHARED_PTR(TimerExecutor);

END_NAMESPACE
#endif
