#ifndef PROCESSORFACTORY_H
#define PROCESSORFACTORY_H

#include "iprocessor.h"
#include "../socket/socketprocessor.h"

namespace cgserver{
    class ProcessorFactory{
    public:
        ProcessorFactory();
        ~ProcessorFactory();

	static IProcessorPtr getProcessor() {
	    return IProcessorPtr((IProcessor *)new SocketProcessor);
	}
    };
}
#endif
