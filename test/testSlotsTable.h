#include <cxxtest/TestSuite.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <slots/data/basicdata.h>

#define AST_EQ(x, y) TS_ASSERT_EQUALS(x, y);
#define AST_TRUE(x) TS_ASSERT(x);

using namespace cgserver;
using namespace slots;


class TestSlotsTables : public CxxTest::TestSuite 
{
public:

    virtual void setUp(){
    }

    virtual void tearDown() {
    }

    void test_Game_Detail( void )
    {
	std::vector<std::string> row;
	row.push_back("76");
	row.push_back("2");
	row.push_back("3");
        row.push_back("123321");
	row.push_back("4");
	row.push_back("5");
	row.push_back("230");
	row.push_back("23");
	row.push_back("56");
	row.push_back("12");
	row.push_back("8");
	row.push_back("9");
	row.push_back("1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16");
	row.push_back("1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16");
	GameHistory gd;
	AST_TRUE(gd.deserialize(row));
	AST_EQ("76", gd.uid);
	AST_EQ(2, gd.friendNum);
	AST_EQ(3, gd.friendGiftsNum);
	AST_EQ(123321, gd.lastLogin);
	AST_EQ(4, gd.consitiveLogin);
	AST_EQ(5, gd.tinyGameTimes);
	AST_EQ(230, gd.bigwin[0]);
	AST_EQ(23, gd.megawin[0]);
	AST_EQ(56, gd.freeTimes[0]);
	AST_EQ(12, gd.gameTimes[0]);
	AST_EQ(8, gd.jackpotTimes[0]);
	AST_EQ(9, gd.gJackpotTimes);
	AST_EQ(9, gd.fourLine[0][8]);
	AST_EQ(13, gd.fiveLine[0][12]);
    }

    void test_cargo_info(void)
    {
            /* std::vector<std::string> in = { */
            /*         "ox12","1000","0,5,15,25","2000","230"}; */
            std::vector<std::string> in;
            in.push_back("ox12");
            in.push_back("1000");
            in.push_back("0,5,15,25");
            in.push_back("2000");
            in.push_back("230");
            CargoInfo ci;
            AST_TRUE(ci.deserialize(in));
            AST_EQ("ox12", ci.cid);
            AST_EQ(1000, ci.base);
            std::cout << ci.vip_extra.size();
            AST_EQ(2000, ci.vip_point);
            AST_EQ(230, ci.free_extra);
            AST_EQ(4, ci.vip_extra.size());
            if (ci.vip_extra.size() < 4) {
                    return ;
            }
            AST_EQ(15, ci.vip_extra[2]);
    }
};
