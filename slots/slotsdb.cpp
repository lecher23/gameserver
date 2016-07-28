#include "slotsdb.h"
namespace slots{
    SlotsDB::SlotsDB(){
    }
    SlotsDB::~SlotsDB(){
    }

    bool SlotsDB::getUserInfo(
	const std::string &mid, UserInfo &ui, std::string &errMsg)
    {
	// get logic
	return true;
    }

    bool SlotsDB::setUserInfo(
	const UserInfo &ui, std::string &errMsg)
    {
	// set logic
	return true;
    }
}
