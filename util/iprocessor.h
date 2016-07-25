#ifndef IPROCESSOR_H
#define IPROCESSOR_H

#include "../socket/socket.h"

namespace cgserver{
    class IProcessor{
    public:
        IProcessor() {}
        virtual ~IProcessor() {};

	virtual void process(Socket *sk) const = 0;
    };
}
#endif
