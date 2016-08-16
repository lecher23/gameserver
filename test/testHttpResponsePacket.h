#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "http/httpresponsepacket.h"

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;

const std::string lua_file = "/home/licheng/workspace/lua/main.lua";

class MyTestSuite : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }
    
    std::string _decrypt(const std::string &str){
	std::string out;
	std::string key = "47611322";
	for (size_t i = 0; i < str.size(); ++i) {
	    out.append(1, (char)(str[i] - key[i % key.size()] + '0'));
	}
	return out;
    }

    void test_encrypt( void )
    {
	std::string str_to_encrypt = "{\"age\":38}";
	HttpResponsePacket resp;
	resp.setBody(str_to_encrypt.c_str(), str_to_encrypt.size());
	std::string out(resp.getBody(), str_to_encrypt.size());
	std::string restore = _decrypt(out);
	std::cout << std::endl << out << std::endl;
	AST_EQ(str_to_encrypt, restore);
    }
};
