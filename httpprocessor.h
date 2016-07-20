#include "runnable.h"
#include "thread.h"

namespace cgserver{
    class Thread;
    class HttpProcessor:Runnable{
    public:
	virtual ~HttpProcessor();
	HttpProcessor();

	virtual void run(Thread *thread, void *arg);
    };
}
