#ifndef IPROCESSOR_H
#define IPROCESSOR_H

#include "../socket/socket.h"

namespace cgserver{
    class IProcessor{
    public:
        IProcessor() {}
        virtual ~IProcessor() {};

	virtual void process(SocketPtr sk) const = 0;
	virtual void init(void *resource) = 0;
    };

    DF_SHARED_PTR(IProcessor);
}
#endif
