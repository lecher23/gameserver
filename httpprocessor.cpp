#include "httpprocessor.h"
#include <iostream>

namespace cgserver{
    HttpProcessor::~HttpProcessor(){;}
    HttpProcessor::HttpProcessor(){;}
    void HttpProcessor::run(Thread *thread, void *arg) {
	std::cout <<  " processing." << std::endl;
    }
}
