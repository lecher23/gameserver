#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <util/stringutil.h>

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;

class TestStringUtil : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_normal( void )
    {
	std::string input = "1,0,3,999,-200";
	int32_t ret[5];
	StringUtil::StrToIntArrayWithDefault(input.c_str(), ',', ret, 5, 9);
	AST_EQ(1, ret[0]);
	AST_EQ(0, ret[1]);
	AST_EQ(3, ret[2]);
	AST_EQ(999, ret[3]);
	AST_EQ(-200, ret[4]);

	int32_t ret2[5];
	StringUtil::StrToIntArrayWithDefault(input.c_str(), input.size(), ',', ret2, 5, 9);
	AST_EQ(1, ret2[0]);
	AST_EQ(0, ret2[1]);
	AST_EQ(3, ret2[2]);
	AST_EQ(999, ret2[3]);
	AST_EQ(-200, ret2[4]);
	
    }

    void test_has_empty_value( void )
    {
	std::string input = "1,300,,,-200";
	int32_t ret[5];
	StringUtil::StrToIntArrayWithDefault(input.c_str(), ',', ret, 5, 9);
	AST_EQ(1, ret[0]);
	AST_EQ(300, ret[1]);
	AST_EQ(0, ret[2]);
	AST_EQ(0, ret[3]);
	AST_EQ(-200, ret[4]);

	int32_t ret2[5];
	StringUtil::StrToIntArrayWithDefault(input.c_str(), input.size(),',', ret2, 5, 9);
	AST_EQ(1, ret2[0]);
	AST_EQ(300, ret2[1]);
	AST_EQ(0, ret2[2]);
	AST_EQ(0, ret2[3]);
	AST_EQ(-200, ret2[4]);
	
    }

    void test_too_much_value( void )
    {
	std::string input = "1,2,3,4,5";
	int32_t ret[4];
	StringUtil::StrToIntArrayWithDefault(input.c_str(), ',', ret, 4, 9);
	AST_EQ(1, ret[0]);
	AST_EQ(2, ret[1]);
	AST_EQ(3, ret[2]);
	AST_EQ(4, ret[3]);

	int32_t ret2[4];
	StringUtil::StrToIntArrayWithDefault(input.c_str(),input.size(), ',', ret2, 4, 9);
	AST_EQ(1, ret2[0]);
	AST_EQ(2, ret2[1]);
	AST_EQ(3, ret2[2]);
	AST_EQ(4, ret2[3]);
	
    }

    void test_less_value( void )
    {
	std::string input = "1,2,";
	int32_t ret[4];
	StringUtil::StrToIntArrayWithDefault(input.c_str(), ',', ret, 4, 9);
	AST_EQ(1, ret[0]);
	AST_EQ(2, ret[1]);
	AST_EQ(0, ret[2]);
	AST_EQ(9, ret[3]);

	int32_t ret2[4];
	StringUtil::StrToIntArrayWithDefault(input.c_str(), ',', ret2, 4, 9);
	AST_EQ(1, ret2[0]);
	AST_EQ(2, ret2[1]);
	AST_EQ(0, ret2[2]);
	AST_EQ(9, ret2[3]);
    }

    void test_normal_multi_array(void)
    {
	std::string input = "1,2;4,5;2,8";
	int32_t ret[3][2];
	int32_t *p[] = {ret[0], ret[1], ret[2]};
	std::cout << sizeof(p) << std::endl;
	StringUtil::StrToIntArray(input.c_str(), ';', ',', p, 3, 2);
	AST_EQ(1, ret[0][0]);	
	AST_EQ(2, ret[0][1]);	
	AST_EQ(4, ret[1][0]);	
	AST_EQ(5, ret[1][1]);	
	AST_EQ(2, ret[2][0]);	
	AST_EQ(8, ret[2][1]);	
    }

    void test_single_row_multi_array(void)
    {
	std::string input = "1,2,3,4,5";
	int32_t ret[1][5];
	int32_t *p[1];	
	for (size_t i = 0; i < 1; ++i){
	    p[i] = ret[i];
	}
	std::cout << sizeof(p) << std::endl;
	StringUtil::StrToIntArray(input.c_str(), ';', ',', p, 1, 5);
	AST_EQ(1, ret[0][0]);	
	AST_EQ(2, ret[0][1]);	
	AST_EQ(3, ret[0][2]);	
	AST_EQ(4, ret[0][3]);	
	AST_EQ(5, ret[0][4]);	
    }
    
    void test_empty_array_to_str(void)
    {
	std::string out;
	int32_t data[3];
	StringUtil::ArrayToStr(data, 0, ',', out);
	AST_TRUE(out.empty());
    }

    void test_single_ele_array_to_str(void)
    {
	std::string out;
	int32_t data[3] = {123, 23, 3};
	StringUtil::ArrayToStr(data, 1, ',', out);
	AST_EQ("123", out);
    }

    void test_multi_ele_array_to_str(void)
    {
	std::string out;
	int32_t data[3] = {123, 23, 3};
	StringUtil::ArrayToStr(data, 3, ',', out);
	AST_EQ("123,23,3", out);
    }
    
    
};
