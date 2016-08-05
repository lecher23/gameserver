#include <cxxtest/TestSuite.h>
#define private public
#include "util/md5tool.h"
#undef private

#include <string.h>
#include <string>

using namespace cgserver;
class TestMd5Tool : public CxxTest::TestSuite 
{
 public:
    void test_update_long_str(void)
    {
	Md5Tool tool;
	std::string ary
	    = "01234567890123456789012345678901234567890123456789012345678901234567890123456789";
	std::string e_16 = "cb01d560d59016a2";
	std::string e_32 = "0faef1f4cb01d560d59016a2d5e91da6";
	tool.update(ary.c_str());
	TS_ASSERT_EQUALS(e_32, tool.toString());
    }
    void test_update_normal_str(void)
    {
	Md5Tool tool;
	std::string ary
	    = "helloworld";
	std::string e_16 = "38a57032085441e7";
	std::string e_32 = "fc5e038d38a57032085441e7fe7010b0";
	tool.update(ary.c_str());
	TS_ASSERT_EQUALS(e_32, tool.toString());
    }

};
