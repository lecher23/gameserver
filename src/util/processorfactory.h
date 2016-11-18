#ifndef PROCESSORFACTORY_H
#define PROCESSORFACTORY_H

#include "iprocessor.h"

namespace cgserver{
    class ProcessorFactory{
    public:
        ProcessorFactory();
        ~ProcessorFactory();

	static IProcessorPtr getProcessor() {
            return IProcessorPtr(nullptr);
	}
    };
}
#endif
